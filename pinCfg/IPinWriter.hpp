#pragma once

namespace pinCfg
{
class IPinWriter
{
  protected:
    IPinWriter(){};

  public:
    virtual ~IPinWriter(){};

    virtual void writePin(bool data) = 0;
};
} // namespace pinCfg
