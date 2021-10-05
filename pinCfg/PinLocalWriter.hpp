#pragma once

#include <cstdint>

#include "IPinWriter.hpp"

namespace pinCfg
{
class PinLocalWriter : public IPinWriter
{
  private:
    const uint8_t outPin_;

  public:
    PinLocalWriter(uint8_t outPin);
    virtual ~PinLocalWriter(){};

    virtual void writePin(bool data) override;
};
} // namespace pinCfg
