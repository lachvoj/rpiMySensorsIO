#pragma once

#include <cstdint>

namespace pinCfg
{
class IPinSubscriber
{
  protected:
    IPinSubscriber(){};

  public:
    virtual void eventHandle(uint8_t eventType, int data) = 0;
    virtual ~IPinSubscriber(){};
};
} // namespace pinCfg
