#ifndef DOUBLEFLASHPATTERN_HPP
#define DOUBLEFLASHPATTERN_HPP
#include "ILedPattern.hpp"

class DoubleFlashPattern : public ILedPattern {
public:
    void run(IBlinker* blinker, int delay_ms, AbortPredicate abort) override {
        for (int i = 0; i < 2; i++) {
            blinker->firstOn();  if (adaptive_delay(60, abort)) return;
            blinker->allOff();   if (adaptive_delay(60, abort)) return;
        }
        for (int i = 0; i < 2; i++) {
            blinker->secondOn(); if (adaptive_delay(60, abort)) return;
            blinker->allOff();   if (adaptive_delay(60, abort)) return;
        }
        blinker->allOff();
        adaptive_delay(delay_ms, abort);
    }
};
#endif // DOUBLEFLASHPATTERN_HPP
