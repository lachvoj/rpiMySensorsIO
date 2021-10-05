#pragma once

#include <cstdint>

#include "IPinWriter.hpp"

namespace pinCfg
{
class PinSPIWriter : public IPinWriter
{
  private:
    const uint8_t outDev_;
    const uint8_t writeByte_; // 7: 0-action 1-noaction; 6: out value (if writing); 5: 0-read 1-write;
                              // 4: 0-digital 1-analog; 0-3: pin number

  public:
    PinSPIWriter(uint8_t outPin, uint8_t outDev = 0);
    virtual ~PinSPIWriter(){};

    virtual void writePin(bool data) override;
};
} // namespace pinCfg
