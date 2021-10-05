#pragma once

#include <cstdint>

#include "IPinReader.hpp"

namespace pinCfg
{
class PinSPIReader : public IPinReader
{
  private:
    const uint8_t inDev_;
    const uint8_t readByte_; // 7: 0-action 1-noaction; 6: out value (if writing); 5: 0-read 1-write;
                             // 4: 0-digital 1-analog; 0-3: pin number

  public:
    PinSPIReader(uint8_t inPin, uint8_t inDev = 0);
    virtual ~PinSPIReader(){};

    virtual bool readPin() override;
};
} // namespace pinCfg
