#pragma once

#include <memory>
#include <string>
#include <vector>

#include "IPinSubscriber.hpp"
#include "Switch.hpp"

using namespace std;

namespace pinCfg
{
class Trigger : public IPinSubscriber
{
  public:
    enum Action
    {
        A_TOGGLE = 0,
        A_UP,
        A_DOWN
    };
    class SwitchAction
    {
      public:
        shared_ptr<Switch> switch_;
        const Trigger::Action action_;
        SwitchAction(shared_ptr<Switch> &sw, Trigger::Action action = Trigger::Action::A_TOGGLE);
    };

  private:
    enum State
    {
        DOWN = 0,
        UP,
        LONG
    };
    vector<Trigger::SwitchAction> switches_;
    const Trigger::State eventType_;
    const uint8_t count_;

  public:
    Trigger(vector<Trigger::SwitchAction> &switches, uint8_t eventType = Trigger::State::UP, uint8_t count = 1);
    void eventHandle(uint8_t eventType, int data) override;
};
} // namespace pinCfg
