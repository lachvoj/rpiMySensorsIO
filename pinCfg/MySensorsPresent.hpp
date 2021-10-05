#pragma once

#include <memory>
#include <string>

#include "ILoopable.hpp"
#include "IMySensorsPresentable.hpp"

using namespace std;

namespace pinCfg
{
class MySensorsPresent : public IMySensorsPresentable
{
  private:
    shared_ptr<MyMessage> myMessage_;
    const int id_;
    const string name_;

  protected:
    int state_ = 0;
    bool stateChanged_ = false;\
    bool present_;

    MySensorsPresent(int id, const string &name, bool present = true);
    void sendMySensorsStatus();

  public:
    virtual ~MySensorsPresent(){};
    int getId() const override;
    const char *getName() const override;
    virtual void rcvMessage(const MyMessage &message);
    virtual void Present() override;
    virtual void PresentState() override;
    virtual void setState(int state);
    virtual void toggle();
};
} // namespace pinCfg
