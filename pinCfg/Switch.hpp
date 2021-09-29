#pragma once

#include <cstdio>
#include <string>

#include "ILoopable.hpp"
#include "MySensorsPresent.hpp"

using namespace std;

namespace pinCfg
{
class Switch : public MySensorsPresent, public ILoopable
{
  public:
    enum Mode
    {
        CLASSIC = 0,
        IMPULSE
    };

  private:
    const Mode mode_;
    const uint8_t outPin_;
    const uint8_t feedbackPin_;

    int impulseDuration_;
    uint64_t impulseStartedMs_;

    bool readFeedbackPin();
    void setOuputPin(bool val);

  public:
    Switch(
        int id,
        const string &name,
        bool present,
        uint8_t outPin,
        Switch::Mode mode = Mode::CLASSIC,
        int impulseDuration = 300,
        uint8_t feedbackPin = 0x80);
    ILoopable::Type getType() override;
    void loop(uint64_t ms) override;
};
} // namespace pinCfg