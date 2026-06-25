#ifndef TWOBUTTONINPUT_HPP
#define TWOBUTTONINPUT_HPP
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "IInputDevice.hpp"

class TwoButtonInput : public IInputDevice {
private:
    static constexpr int DEBOUNCE_MS = 30;
    static constexpr int LONG_PRESS_MS = 600;

    struct Button {
        gpio_num_t pin;
        bool active_high;
        bool down = false;
        TickType_t press_tick = 0;
        TickType_t last_change = 0;

        int pressedLevel() const { return active_high ? 1 : 0; }
    };

    Button m_a;
    Button m_b;

    static void configPin(const Button &btn, gpio_isr_t isr, void *isr_arg) {
        gpio_config_t io = {};
        io.pin_bit_mask = (1ULL << btn.pin);
        io.mode = GPIO_MODE_INPUT;
        io.pull_up_en = btn.active_high ? GPIO_PULLUP_DISABLE : GPIO_PULLUP_ENABLE;
        io.pull_down_en = btn.active_high ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
        io.intr_type = GPIO_INTR_ANYEDGE;
        gpio_config(&io);
        gpio_isr_handler_add(btn.pin, isr, isr_arg);
    }


    Event processButton(Button &btn, Event shortEvent) {
        bool pressed = (gpio_get_level(btn.pin) == btn.pressedLevel());
        TickType_t now = xTaskGetTickCount();

        if ((now - btn.last_change) < pdMS_TO_TICKS(DEBOUNCE_MS)) {
            return Event::None;
        }

        if (pressed && !btn.down) {
            btn.down = true;
            btn.press_tick = now;
            btn.last_change = now;
        } else if (!pressed && btn.down) {
            btn.down = false;
            btn.last_change = now;
            TickType_t held = now - btn.press_tick;
            return (held >= pdMS_TO_TICKS(LONG_PRESS_MS)) ? Event::NextPattern : shortEvent;
        }
        return Event::None;
    }

public:
    TwoButtonInput(gpio_num_t pin_a, bool a_active_high,
                   gpio_num_t pin_b, bool b_active_high)
        : m_a{pin_a, a_active_high}, m_b{pin_b, b_active_high} {
    }

    void init(gpio_isr_t isr, void *isr_arg) override {
        gpio_install_isr_service(0);
        configPin(m_a, isr, isr_arg);
        configPin(m_b, isr, isr_arg);
    }

    Event poll() override {
        Event ev = processButton(m_a, Event::DecreaseDelay);
        if (ev == Event::None) {
            ev = processButton(m_b, Event::IncreaseDelay);
        }
        return ev;
    }
};
#endif // TWOBUTTONINPUT_HPP
