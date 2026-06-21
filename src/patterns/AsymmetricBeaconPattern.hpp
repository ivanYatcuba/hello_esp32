#ifndef ASYMMETRICBEACONPATTERN_HPP
#define ASYMMETRICBEACONPATTERN_HPP
#include "ILedPattern.hpp"

class AsymmetricBeaconPattern : public ILedPattern {
public:
    void run(IBlinker* blinker, int delay_ms, AbortPredicate abort) override {
        for (int i = 0; i < 3; i++) {
            blinker->firstOn();
            if (adaptive_delay(80, abort)) return;
            blinker->allOff();
            if (adaptive_delay(80, abort)) return;
        }

        if (adaptive_delay(200, abort)) return;

        blinker->secondOn();
        if (adaptive_delay(delay_ms, abort)) return;
        blinker->allOff();

        adaptive_delay(delay_ms, abort);
    }
};
#endif // ASYMMETRICBEACONPATTERN_HPP
