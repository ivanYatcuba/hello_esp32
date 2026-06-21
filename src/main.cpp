#include <cstdio>
#include <iterator>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "blinkers/BlinkerFactory.hpp"
#include "EncoderState.hpp"
#include "patterns/ClassicBlinkPattern.hpp"
#include "patterns/DoubleFlashPattern.hpp"
#include "patterns/AsymmetricBeaconPattern.hpp"

#define BLINK_GPIO GPIO_NUM_4

#define ENCODER_A_GPIO GPIO_NUM_18
#define ENCODER_B_GPIO GPIO_NUM_17
#define ENCODER_BUTTON_GPIO GPIO_NUM_16

static IBlinker* myBlinker = nullptr;

static ClassicBlinkPattern     classicPattern;
static DoubleFlashPattern      doubleFlashPattern;
static AsymmetricBeaconPattern beaconPattern;

static ILedPattern* patterns[] = {
    &classicPattern,
    &doubleFlashPattern,
    &beaconPattern
};
static constexpr int PATTERN_COUNT = std::size(patterns);

static EncoderState encoder(ENCODER_A_GPIO, ENCODER_B_GPIO, ENCODER_BUTTON_GPIO, PATTERN_COUNT);

static TaskHandle_t encoder_task_handle = nullptr;

static void IRAM_ATTR encoder_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(encoder_task_handle, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

[[noreturn]] static void encoder_task(void *pvParameters)
{
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        encoder.update();
    }
}

static int active_pattern = -1;

[[noreturn]] static void led_task(void *pvParameters)
{
    while (true) {
        active_pattern = encoder.pattern();
        patterns[active_pattern]->run(
            myBlinker, encoder.delayMs(),
            [] { return encoder.pattern() != active_pattern; });
    }
}

extern "C" void app_main(void)
{
    myBlinker = BlinkerFactory::createBlinker(BlinkerType::SINGLE_PIN_HARDWARE_TRICK, BLINK_GPIO);

    if (myBlinker != nullptr) {
        myBlinker->init();
    }

    xTaskCreate(encoder_task, "encoder_task", 3072, nullptr, 3, &encoder_task_handle);
    xTaskCreate(led_task, "led_task", 3072, nullptr, 2, nullptr);

    encoder.init(encoder_isr_handler, nullptr);
}
