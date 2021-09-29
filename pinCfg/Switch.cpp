#include <Arduino.h>

#include "Switch.hpp"

namespace pinCfg
{
Switch::Switch(
    int id,
    const string &name,
    bool present,
    uint8_t outPin,
    Switch::Mode mode,
    int impulseDuration,
    uint8_t feedbackPin)
: MySensorsPresent(id, name, present), mode_(mode), outPin_(outPin), feedbackPin_(feedbackPin),
  impulseDuration_(impulseDuration)
{
    pinMode(outPin_, BCM2835_GPIO_FSEL_OUTP);
    if (feedbackPin_ != 0)
        pinMode(feedbackPin_, BCM2835_GPIO_FSEL_INPT);
}

ILoopable::Type Switch::getType()
{
    return ILoopable::Type::SWITCH;
}

void Switch::loop(uint64_t ms)
{
    if (feedbackPin_ != 0)
    {
        state_ = (int)readFeedbackPin();
    }

    if (!stateChanged_)
        return;

    if (mode_ == Mode::IMPULSE)
    {
        if (impulseStartedMs_ == 0)
        {
            impulseStartedMs_ = ms;
            setOuputPin(true);
        }
        else if ((ms - impulseStartedMs_) >= impulseDuration_)
        {
            impulseStartedMs_ = 0;
            stateChanged_ = false;
            setOuputPin(false);
        }
    }
    else if (mode_ == Mode::CLASSIC)
    {
        stateChanged_ = false;
        setOuputPin((bool)state_);
    }
}

bool Switch::readFeedbackPin()
{
    return (bool)digitalRead(feedbackPin_);
}

void Switch::setOuputPin(bool val)
{
    digitalWrite(outPin_, (uint8_t)val);
}

} // namespace pinCfg
