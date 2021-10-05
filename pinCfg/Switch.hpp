#pragma once

#include <cstdio>
#include <memory>
#include <string>

#include "ILoopable.hpp"
#include "IPinReader.hpp"
#include "IPinWriter.hpp"
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

    int impulseDuration_;
    uint64_t impulseStartedMs_;

    unique_ptr<IPinWriter> pinWriter_;
    unique_ptr<IPinReader> fbPinReader_;

  public:
    Switch(
        int id,
        const string &name,
        bool present,
        Switch::Mode mode = Mode::CLASSIC,
        uint8_t outPin = 0,
        uint8_t outDevice = 0xFF,
        uint8_t feedbackPin = 0,
        uint8_t feedbackDev = 0xFF,
        int impulseDuration = 300);
    void loop(uint64_t ms) override;
};
} // namespace pinCfg
