#pragma once

#include <string>

#include "IMySensorsPresentable.hpp"

using namespace std;

namespace pinCfg
{
class ExtCfgReciever : public IMySensorsPresentable
{
  private:
    static const string configEnd;
    MyMessage myMessage_;
    const int id_;
    const string name_;
    string receivingConfig_;
    string receivedConfig_;

    bool endsWith(const string& str, const string& ending);

  public:
    ExtCfgReciever(int id, const string &name);
    int getId() const;
    const char *getName() const;
    void rcvMessage(const MyMessage &message);
    void Present();
    void PresentState();
};
} // namespace pinCfg
