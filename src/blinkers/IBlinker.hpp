#ifndef UNTITLED_IBLINKER_H
#define UNTITLED_IBLINKER_H

class IBlinker {
public:
    virtual ~IBlinker() = default;

    virtual void init() = 0;

    virtual void firstOn() = 0;

    virtual void secondOn() = 0;

    virtual void allOff() = 0;

    virtual int maxBrightness() = 0;

    virtual void setBrightness(uint8_t level) { level > 0 ? firstOn() : allOff(); }
};

#endif //UNTITLED_IBLINKER_H
