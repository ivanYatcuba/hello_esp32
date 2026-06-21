#ifndef SINGLEPINBLINKER_HPP
#define SINGLEPINBLINKER_HPP
#include "IBlinker.hpp"
#include "driver/gpio.h"

class SinglePinBlinker : public IBlinker {
private:
    gpio_num_t m_pin;

public:
    SinglePinBlinker(gpio_num_t pin) : m_pin(pin) {}

    void init() override {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT_OUTPUT; 
        io_conf.pin_bit_mask = (1ULL << m_pin);
        gpio_config(&io_conf);
        allOff();
    }

    void firstOn() override {
        gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(m_pin, 1);
    }

    void secondOn() override {
        gpio_set_direction(m_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(m_pin, 0);
    }

    void allOff() override {
        gpio_set_direction(m_pin, GPIO_MODE_INPUT);
    }
};
#endif // SINGLEPINBLINKER_HPP
