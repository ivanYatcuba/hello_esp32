#ifndef ILEDPATTERN_HPP
#define ILEDPATTERN_HPP
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "blinkers/IBlinker.hpp"

using AbortPredicate = bool (*)();

inline bool adaptive_delay(int total_ms, AbortPredicate abort)
{
    for (int elapsed = 0; elapsed < total_ms; elapsed += 10) {
        if (abort && abort()) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return false;
}

class ILedPattern {
public:
    virtual ~ILedPattern() = default;

    virtual void run(IBlinker* blinker, int delay_ms, AbortPredicate abort) = 0;
};
#endif // ILEDPATTERN_HPP
