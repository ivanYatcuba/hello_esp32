#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <cstdio>
#include <atomic>
#include <algorithm>

class Controls {
public:
    enum class Event { None, NextPattern, IncreaseDelay, DecreaseDelay };

    explicit Controls(int pattern_count) : m_pattern_count(pattern_count) {
    }

    void apply(Event ev) {
        switch (ev) {
            case Event::NextPattern:
                m_pattern = (m_pattern + 1) % m_pattern_count;
                printf("Pattern -> %d\n", m_pattern.load());
                break;
            case Event::IncreaseDelay:
            case Event::DecreaseDelay:
                m_delay_ms = std::clamp(m_delay_ms + (ev == Event::IncreaseDelay ? DELAY_STEP : -DELAY_STEP),
                                        DELAY_MIN, DELAY_MAX);
                printf("Delay -> %d ms\n", m_delay_ms.load());
                break;
            case Event::None:
                break;
        }
    }

    [[nodiscard]] int delayMs() const { return m_delay_ms; }
    [[nodiscard]] int pattern() const { return m_pattern; }

private:
    static constexpr int DELAY_MIN = 100, DELAY_MAX = 3000, DELAY_STEP = 100;

    int m_pattern_count;
    std::atomic<int> m_delay_ms{500};
    std::atomic<int> m_pattern{0};
};
#endif // CONTROLS_HPP
