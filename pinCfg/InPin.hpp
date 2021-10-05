#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "ILoopable.hpp"
#include "IPinSubscriber.hpp"
#include "MySensorsPresent.hpp"

using namespace std;

namespace pinCfg
{
class InPin : public MySensorsPresent, public ILoopable
{
  private:
    enum PinState
    {
        DOWN = 0,
        UP,
        LONG,
        MULTI,
        DEBOUNCEDOWN,
        DEBOUNCEUP
    };
    const uint8_t inputPin_;
    PinState pinState_ = DOWN;
    vector<shared_ptr<IPinSubscriber>> subscribers_;
    static uint64_t debounceMs;
    static uint64_t multiclickMaxDelayMs;
    bool lastPinState_ = false;
    uint8_t pressCount_ = 0;
    uint64_t timerDebounceStarted_ = UINT64_MAX;
    uint64_t timerMultiStarted_ = UINT64_MAX;

    void sendEvent(uint8_t eventType, int data = 0);

  public:
    static void setDebounceMs(uint64_t debounce);
    static void setMulticlickMaxDelayMs(uint64_t multikMaxDelay);

    InPin(int id, const string &name, bool present, uint8_t inputPin);
    uint8_t getInputPin() const;
    void addSubscriber(shared_ptr<IPinSubscriber> &subscriber);
    void setState(int state) override;
    void loop(uint64_t ms) override;
};

} // namespace pinCfg
