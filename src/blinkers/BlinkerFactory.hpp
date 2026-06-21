#ifndef BLINKERFACTORY_HPP
#define BLINKERFACTORY_HPP
#include "SinglePinBlinker.hpp"

enum class BlinkerType {
    SINGLE_PIN_HARDWARE_TRICK,
    STANDARD_TWO_PINS //todo
};

class BlinkerFactory {
public:
    static IBlinker* createBlinker(const BlinkerType type, const gpio_num_t primary_pin) {
        switch (type) {
            case BlinkerType::SINGLE_PIN_HARDWARE_TRICK:
                return new SinglePinBlinker(primary_pin);
            default:
                return nullptr;
        }
    }
};
#endif // BLINKERFACTORY_HPP
