#include "PinLocalReader.hpp"
#include "PinLocalWriter.hpp"
#include "PinSPIReader.hpp"
#include "PinSPIWriter.hpp"
#include "Switch.hpp"

namespace pinCfg
{
Switch::Switch(
    int id,
    const string &name,
    bool present,
    Switch::Mode mode,
    uint8_t outPin,
    uint8_t outDevice,
    uint8_t feedbackPin,
    uint8_t feedbackDev,
    int impulseDuration)
: MySensorsPresent(id, name, present), mode_(mode), impulseDuration_(impulseDuration)
{
    if (outDevice == 0xFF)
        pinWriter_ = unique_ptr<PinLocalWriter>(new PinLocalWriter(outPin));
    else
        pinWriter_ = unique_ptr<PinSPIWriter>(new PinSPIWriter(outPin, outDevice));

    if (feedbackPin != 0)
    {
        if (feedbackDev == 0xFF)
            fbPinReader_ = unique_ptr<PinLocalReader>(new PinLocalReader(feedbackPin));
        else
            fbPinReader_ = unique_ptr<PinSPIReader>(new PinSPIReader(feedbackPin, feedbackDev));
    }
}

void Switch::loop(uint64_t ms)
{
    if (fbPinReader_)
    {
        state_ = static_cast<int>(fbPinReader_->readPin());
    }

    if (!stateChanged_)
        return;

    if (mode_ == Mode::IMPULSE)
    {
        if (impulseStartedMs_ == 0)
        {
            impulseStartedMs_ = ms;
            pinWriter_->writePin(true);
        }
        else if ((ms - impulseStartedMs_) >= impulseDuration_)
        {
            impulseStartedMs_ = 0;
            stateChanged_ = false;
            pinWriter_->writePin(false);
        }
    }
    else if (mode_ == Mode::CLASSIC)
    {
        stateChanged_ = false;
        pinWriter_->writePin((bool)state_);
    }
}
} // namespace pinCfg
