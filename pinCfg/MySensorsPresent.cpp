#include <MySensorsCore.h>

#include "MySensorsPresent.hpp"

namespace pinCfg
{
MySensorsPresent::MySensorsPresent(int id, const string &name, bool present) : id_(id), name_(name), present_(present)
{
    if (present_)
        myMessage_ = make_shared<MyMessage>(id_, V_STATUS);
}

// IMySensorsPresentable
int MySensorsPresent::getId() const
{
    return id_;
}

const char *MySensorsPresent::getName() const
{
    return name_.c_str();
}

void MySensorsPresent::rcvMessage(const MyMessage &message)
{
    setState(message.getInt());
}

void MySensorsPresent::Present()
{
    present(id_, S_BINARY, name_.c_str());
}

void MySensorsPresent::PresentState()
{
    sendMySensorsStatus();
    request(static_cast<uint8_t>(id_), V_STATUS);
}

// public
void MySensorsPresent::setState(int state)
{
    stateChanged_ = (state_ != state);
    state_ = state;
    if (stateChanged_)
        sendMySensorsStatus();
}

void MySensorsPresent::toggle()
{
    state_ = state_ ? (int)false : (int)true;
    stateChanged_ = true;
    sendMySensorsStatus();
}

// protected
void MySensorsPresent::sendMySensorsStatus()
{
    if (!myMessage_)
        return;

    myMessage_->set(state_);
    send(*myMessage_);
}
} // namespace pinCfg
