#ifndef ENCODERINPUT_HPP
#define ENCODERINPUT_HPP
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "IInputDevice.hpp"

class EncoderInput : public IInputDevice {
private:
    gpio_num_t m_pin_a;
    gpio_num_t m_pin_b;
    gpio_num_t m_pin_button;

    int m_last_encoder_a    = 0;
    int m_last_button_state = 1;
    TickType_t m_last_button_time = 0;

public:
    EncoderInput(gpio_num_t pin_a, gpio_num_t pin_b, gpio_num_t pin_button)
        : m_pin_a(pin_a), m_pin_b(pin_b), m_pin_button(pin_button) {}

    void init(gpio_isr_t isr, void *isr_arg) override
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

    Event poll() override
    {
        int button_state = gpio_get_level(m_pin_button);
        if (m_last_button_state == 1 && button_state == 0) {
            m_last_button_state = button_state;
            TickType_t now = xTaskGetTickCount();
            if ((now - m_last_button_time) > pdMS_TO_TICKS(200)) {
                m_last_button_time = now;
                return Event::NextPattern;
            }
        } else {
            m_last_button_state = button_state;
        }

        int encoder_a = gpio_get_level(m_pin_a);
        if (encoder_a != m_last_encoder_a) {
            m_last_encoder_a = encoder_a;
            if (encoder_a == 0) {
                return (gpio_get_level(m_pin_b) == 1) ? Event::IncreaseDelay
                                                      : Event::DecreaseDelay;
            }
        }

        return Event::None;
    }
};
#endif // ENCODERINPUT_HPP
