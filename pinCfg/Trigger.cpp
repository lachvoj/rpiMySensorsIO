#include "Trigger.hpp"

namespace pinCfg
{
Trigger::Trigger(vector<Trigger::SwitchAction> &switches, uint8_t eventType, uint8_t count)
: switches_(std::move(switches)), eventType_((Trigger::State)eventType), count_(count)
{
}

void Trigger::eventHandle(uint8_t eventType, int data)
{
    if ((Trigger::State)eventType != eventType_ || count_ != (uint8_t)data)
        return;

    for (auto &sw : switches_)
    {
        switch (sw.action_)
        {
        default:
        case Trigger::Action::A_TOGGLE: sw.switch_->toggle(); break;
        case Trigger::Action::A_UP: sw.switch_->setState((int)true); break;
        case Trigger::Action::A_DOWN: sw.switch_->setState((int)false); break;
        }
    }
}

Trigger::SwitchAction::SwitchAction(shared_ptr<Switch> &sw, Trigger::Action action) : switch_(sw), action_(action)
{
}
} // namespace pinCfg
