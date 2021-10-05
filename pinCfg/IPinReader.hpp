#pragma once

namespace pinCfg
{
class IPinReader
{
  protected:
    IPinReader(){};

  public:
    virtual ~IPinReader(){};

    virtual bool readPin() = 0;
};
} // namespace pinCfg
