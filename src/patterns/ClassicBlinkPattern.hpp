#ifndef CLASSICBLINKPATTERN_HPP
#define CLASSICBLINKPATTERN_HPP
#include "ILedPattern.hpp"

class ClassicBlinkPattern : public ILedPattern {
public:
    void run(IBlinker* blinker, int delay_ms, AbortPredicate abort) override {
        blinker->firstOn();
        if (adaptive_delay(delay_ms, abort)) return;

        blinker->secondOn();
        if (adaptive_delay(delay_ms, abort)) return;
    }
};
#endif // CLASSICBLINKPATTERN_HPP
