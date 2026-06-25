#ifndef TRICKBLINKER_HPP
#define TRICKBLINKER_HPP
#include "IBlinker.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"

class SinglePinBlinker : public IBlinker {
private:
    gpio_num_t m_pin;
    bool m_pwm = false;

    static constexpr ledc_mode_t MODE = LEDC_LOW_SPEED_MODE;
    static constexpr ledc_timer_t TIMER = LEDC_TIMER_1;
    static constexpr ledc_channel_t CH = LEDC_CHANNEL_1;
    static constexpr ledc_timer_bit_t RES = LEDC_TIMER_8_BIT;
    static constexpr uint32_t FREQ = 5000;

    void usePwm() {
        if (m_pwm) return;
        ledc_channel_config_t channel = {};
        channel.gpio_num = m_pin;
        channel.speed_mode = MODE;
        channel.channel = CH;
        channel.timer_sel = TIMER;
        channel.duty = 0;
        channel.hpoint = 0;
        ledc_channel_config(&channel);
        m_pwm = true;
    }

    void useGpio() {
        if (!m_pwm) return;
        ledc_stop(MODE, CH, 0);
        gpio_reset_pin(m_pin);
        gpio_pullup_dis(m_pin);
        gpio_pulldown_dis(m_pin);
        m_pwm = false;
    }

public:
    explicit SinglePinBlinker(gpio_num_t pin) : m_pin(pin) {
    }

    void init() override {
        ledc_timer_config_t timer = {};
        timer.speed_mode = MODE;
        timer.duty_resolution = RES;
        timer.timer_num = TIMER;
        timer.freq_hz = FREQ;
        timer.clk_cfg = LEDC_AUTO_CLK;
        ledc_timer_config(&timer);
        allOff();
    }

    void firstOn() override {
        useGpio();
        gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(m_pin, 1);
    }

    void secondOn() override {
        useGpio();
        gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(m_pin, 0);
    }

    void allOff() override {
        useGpio();
        gpio_set_direction(m_pin, GPIO_MODE_DISABLE);
    }

    void setBrightness(uint8_t level) override {
        usePwm();
        ledc_set_duty(MODE, CH, level);
        ledc_update_duty(MODE, CH);
    }

    int maxBrightness() override {
        return (1 << static_cast<int>(RES)) - 1;
    }
};
#endif // TRICKBLINKER_HPP
