#pragma once

#include <cstdint>

#include "IPinReader.hpp"

namespace pinCfg
{
class PinLocalReader : public IPinReader
{
  private:
    const uint8_t inPin_;

  public:
    PinLocalReader(uint8_t inPin);
    virtual ~PinLocalReader(){};

    virtual bool readPin() override;
};
} // namespace pinCfg
