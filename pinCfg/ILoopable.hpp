#pragma once

#include <cstdint>

namespace pinCfg
{
class ILoopable
{
  protected:
    ILoopable(){};

  public:
    enum Type
    {
        SWITCH = 0,
        PIN
    };
    virtual ~ILoopable(){};
    virtual void loop(uint64_t ms) = 0;
    virtual ILoopable::Type getType() = 0;
};

} // namespace pinCfg