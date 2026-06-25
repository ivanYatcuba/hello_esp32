#include <cstdio>
#include <iterator>

#include "blinkers/BlinkerFactory.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "input/Controls.hpp"
#include "input/EncoderInput.hpp"
#include "input/TwoButtonInput.hpp"
#include "patterns/ClassicBlinkPattern.hpp"
#include "patterns/DoubleFlashPattern.hpp"
#include "patterns/ColorMixPattern.hpp"

#define BLINK_GPIO GPIO_NUM_4

#define BUTTON_A GPIO_NUM_0
#define BUTTON_B GPIO_NUM_21

#define ENCODER_A_GPIO GPIO_NUM_18
#define ENCODER_B_GPIO GPIO_NUM_17
#define ENCODER_BUTTON_GPIO GPIO_NUM_16

static IBlinker* myBlinker = nullptr;

static ClassicBlinkPattern     classicPattern;
static DoubleFlashPattern      doubleFlashPattern;
static ColorMixPattern         colorMixPattern;

static ILedPattern* patterns[] = {
    &classicPattern,
    &doubleFlashPattern,
    &colorMixPattern
};
static constexpr int PATTERN_COUNT = std::size(patterns);

static Controls controls(PATTERN_COUNT);

static EncoderInput encoderInput(ENCODER_A_GPIO, ENCODER_B_GPIO, ENCODER_BUTTON_GPIO);
static TwoButtonInput twoButtonInput(BUTTON_A, /*active_high=*/false,
                                     BUTTON_B, /*active_high=*/true);
static IInputDevice* inputDevice = &twoButtonInput;

static TaskHandle_t input_task_handle = nullptr;

static void IRAM_ATTR input_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(input_task_handle, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

[[noreturn]] static void input_task(void *pvParameters)
{
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        for (Event ev = inputDevice->poll(); ev != Event::None; ev = inputDevice->poll()) {
            printf("Event: %d\n", static_cast<int>(ev));
            controls.apply(ev);
        }
    }
}

static int active_pattern = -1;

[[noreturn]] static void led_task(void *pvParameters)
{
    while (true) {
        active_pattern = controls.pattern();

        patterns[active_pattern]->run(
            myBlinker, controls.delayMs(),
            [] { return controls.pattern() != active_pattern; });

        myBlinker->allOff();
    }
}

extern "C" void app_main(void)
{
    myBlinker = BlinkerFactory::createBlinker(BlinkerType::SINGLE_PIN_HARDWARE_TRICK, BLINK_GPIO);

    if (myBlinker != nullptr) {
        myBlinker->init();
    }

    xTaskCreate(input_task, "input_task", 3072, nullptr, 3, &input_task_handle);
    xTaskCreate(led_task, "led_task", 3072, nullptr, 2, nullptr);

    inputDevice->init(input_isr_handler, nullptr);
}
