#ifndef IINPUTDEVICE_HPP
#define IINPUTDEVICE_HPP
#include "driver/gpio.h"
#include "Controls.hpp"

using Event = Controls::Event;

class IInputDevice {
public:
    virtual ~IInputDevice() = default;
    virtual void init(gpio_isr_t isr, void *isr_arg) = 0;
    virtual Event poll() = 0;
};
#endif // IINPUTDEVICE_HPP
