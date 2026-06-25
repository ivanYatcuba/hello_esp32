#ifndef COLORMIXPATTERN_HPP
#define COLORMIXPATTERN_HPP
#include "ILedPattern.hpp"

class ColorMixPattern : public ILedPattern {
public:
    void run(IBlinker* blinker, int delay_ms, AbortPredicate abort) override {
        const int maxLevel = blinker->maxBrightness();
        const int steps    = maxLevel / STEP;
        int step_ms = delay_ms / steps;
        if (step_ms < 10) step_ms = 10;

        for (int duty = 0; duty <= maxLevel; duty += STEP) {
            blinker->setBrightness(static_cast<uint8_t>(duty));
            if (adaptive_delay(step_ms, abort)) return;
        }
        for (int duty = maxLevel; duty >= 0; duty -= STEP) {
            blinker->setBrightness(static_cast<uint8_t>(duty));
            if (adaptive_delay(step_ms, abort)) return;
        }
    }

private:
    static constexpr int STEP = 5;
};
#endif // COLORMIXPATTERN_HPP
