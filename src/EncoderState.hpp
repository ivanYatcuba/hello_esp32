#ifndef ENCODERSTATE_HPP
#define ENCODERSTATE_HPP
#include <cstdio>
#include <atomic>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class EncoderState {
private:
    gpio_num_t m_pin_a;
    gpio_num_t m_pin_b;
    gpio_num_t m_pin_button;

    int m_pattern_count;

    std::atomic<int> m_delay_ms{500};
    std::atomic<int> m_pattern{0};

    int m_last_encoder_a    = 0;
    int m_last_button_state = 1;
    TickType_t m_last_button_time = 0;

public:
    EncoderState(gpio_num_t pin_a, gpio_num_t pin_b, gpio_num_t pin_button, int pattern_count)
        : m_pin_a(pin_a), m_pin_b(pin_b), m_pin_button(pin_button), m_pattern_count(pattern_count) {}

    void init(gpio_isr_t isr, void *isr_arg)
    {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << m_pin_a) | (1ULL << m_pin_b) | (1ULL << m_pin_button),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_ANYEDGE
        };
        gpio_config(&io_conf);

        gpio_install_isr_service(0);
        gpio_isr_handler_add(m_pin_a, isr, isr_arg);
        gpio_isr_handler_add(m_pin_b, isr, isr_arg);
        gpio_isr_handler_add(m_pin_button, isr, isr_arg);

        m_last_encoder_a = gpio_get_level(m_pin_a);
    }

    void update()
    {
        int button_state = gpio_get_level(m_pin_button);
        if (m_last_button_state == 1 && button_state == 0) {
            TickType_t now = xTaskGetTickCount();
            if ((now - m_last_button_time) > pdMS_TO_TICKS(200)) {
                m_pattern = (m_pattern + 1) % m_pattern_count;
                printf("Pattern changed to %d\n", m_pattern.load());
                m_last_button_time = now;
            }
        }
        m_last_button_state = button_state;

        int encoder_a = gpio_get_level(m_pin_a);
        if (encoder_a != m_last_encoder_a && encoder_a == 0) {
            if (gpio_get_level(m_pin_b) == 1) {
                m_delay_ms = (m_delay_ms >= 3000) ? 3000 : m_delay_ms + 100;
            } else {
                m_delay_ms = (m_delay_ms <= 100) ? 100 : m_delay_ms - 100;
            }
            printf("Delay changed to %d ms\n", m_delay_ms.load());
        }
        m_last_encoder_a = encoder_a;
    }

    [[nodiscard]] int delayMs() const { return m_delay_ms; }
    [[nodiscard]] int pattern() const { return m_pattern; }
};
#endif // ENCODERSTATE_HPP
