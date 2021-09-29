#include <MySensorsCore.h>
#include <iostream>

#include "ExtCfgReciever.hpp"

namespace pinCfg
{

const string ExtCfgReciever::configEnd = "//#END#";

ExtCfgReciever::ExtCfgReciever(int id, const string &name) : id_(id), name_(name), myMessage_(id, V_TEXT)
{
}

int ExtCfgReciever::getId() const
{
    return id_;
}

const char *ExtCfgReciever::getName() const
{
    return name_.c_str();
}

void ExtCfgReciever::rcvMessage(const MyMessage &message)
{
    const char *cfg = message.getString();
    receivingConfig_ += cfg;
    if (endsWith(receivingConfig_, configEnd)) {
        receivedConfig_ = receivingConfig_.substr(0, receivingConfig_.length() - configEnd.length());
        receivingConfig_.clear();
        cout << receivedConfig_ << endl;
    }
}

void ExtCfgReciever::Present()
{
    present(id_, S_INFO, name_.c_str());
}

void ExtCfgReciever::PresentState()
{
    send(myMessage_.set("-"));
    request(static_cast<uint8_t>(id_), V_TEXT);
}

// private
bool ExtCfgReciever::endsWith(const string &str, const string &ending)
{
    if (ending.length() > str.length())
        return false;

    return (0 == str.compare(str.length() - ending.length(), ending.length(), ending));
}
} // namespace pinCfg
