#include <Arduino.h>

#include "InPin.hpp"
#include "PinLocalReader.hpp"
#include "PinSPIReader.hpp"

namespace pinCfg
{
uint64_t InPin::debounceMs;
uint64_t InPin::multiclickMaxDelayMs;

void InPin::setDebounceMs(uint64_t debounce)
{
    InPin::debounceMs = debounce;
}

void InPin::setMulticlickMaxDelayMs(uint64_t multikMaxDelay)
{
    InPin::multiclickMaxDelayMs = multikMaxDelay;
}

InPin::InPin(int id, const string &name, bool present, uint8_t inputPin, uint8_t inputDevice)
: MySensorsPresent(id, name, present), lastPinState_(true)
{
    state_ = false;
    if (inputDevice == 0xFF)
        pinReader_ = unique_ptr<PinLocalReader>(new PinLocalReader(inputPin));
    else
        pinReader_ = unique_ptr<PinSPIReader>(new PinSPIReader(inputPin, inputDevice));
}

void InPin::addSubscriber(shared_ptr<IPinSubscriber> &subscriber)
{
    subscribers_.push_back(subscriber);
}

void InPin::setState(int state)
{
    stateChanged_ = (state_ != state);
    if (stateChanged_)
        sendMySensorsStatus();
}

void InPin::loop(uint64_t ms)
{
    enum Change
    {
        NOCHANGE = 0,
        UP,
        DOWN
    };
    Change change = Change::NOCHANGE;

    if (pinState_ != PinState::DEBOUNCEDOWN && pinState_ != PinState::DEBOUNCEUP)
    {
        bool pinState = static_cast<bool>(pinReader_->readPin());
        if (pinState != lastPinState_)
            change = pinState ? Change::DOWN : Change::UP; // pull up inverted logic!!!
        lastPinState_ = pinState;
    }

    switch (pinState_)
    {
    case PinState::DEBOUNCEDOWN:
    {
        if (ms - timerDebounceStarted_ < debounceMs)
            break;

        pinState_ = PinState::DOWN;
        sendEvent(pinState_);
        MySensorsPresent::setState((int)false);
    }
    break;
    case PinState::DOWN:
    {
        if (pressCount_ > 0 && (ms - timerMultiStarted_) > multiclickMaxDelayMs)
        {
            sendEvent(PinState::MULTI, (int)pressCount_);
            pressCount_ = 0;
        }
        if (change == Change::UP)
        {
            pinState_ = PinState::DEBOUNCEUP;
            timerDebounceStarted_ = ms;
        }
    }
    break;
    case PinState::DEBOUNCEUP:
    {
        if (ms - timerDebounceStarted_ < debounceMs)
            break;

        pinState_ = PinState::UP;
        timerMultiStarted_ = ms;
        pressCount_++;
        sendEvent(pinState_, (int)pressCount_);
        MySensorsPresent::setState((int)true);
    }
    break;
    case PinState::UP:
    {
        if (ms - timerMultiStarted_ > multiclickMaxDelayMs)
        {
            pinState_ = PinState::LONG;
            sendEvent(pinState_);
            pressCount_ = 0;
        }
        else if (change == Change::DOWN)
        {
            pinState_ = PinState::DEBOUNCEDOWN;
            timerDebounceStarted_ = ms;
        }
    }
    break;
    case PinState::LONG:
    {
        if (change == Change::DOWN)
        {
            pinState_ = PinState::DEBOUNCEDOWN;
            timerDebounceStarted_ = ms;
        }
    }
    break;
    default: break;
    }
}

void InPin::sendEvent(uint8_t eventType, int data)
{
    for (auto &subscriber : subscribers_)
        subscriber->eventHandle(eventType, data);
}
} // namespace pinCfg