#pragma once

#include "MyMessage.h"

using namespace std;

namespace pinCfg
{
class IMySensorsPresentable
{
  private:
  protected:
    IMySensorsPresentable(){};

  public:
    virtual ~IMySensorsPresentable(){};
    virtual int getId() const = 0;
    virtual const char *getName() const = 0;
    virtual void rcvMessage(const MyMessage &message) = 0;
    virtual void Present() = 0;
    virtual void PresentState() = 0;
};
} // namespace pinCfg