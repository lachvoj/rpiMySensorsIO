#pragma once

#include <cstdint>

namespace pinCfg
{
class ILoopable
{
  protected:
    ILoopable(){};

  public:
    virtual ~ILoopable(){};
    virtual void loop(uint64_t ms) = 0;
};

} // namespace pinCfg