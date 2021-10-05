#include "PinSPIWriter.hpp"
#include "SPIArduino.hpp"

namespace pinCfg
{
PinSPIWriter::PinSPIWriter(uint8_t outPin, uint8_t outDev) : outDev_(outDev), writeByte_(0b00100000 | (0x0F & outPin))
{
}

void PinSPIWriter::writePin(bool data)
{
    uint8_t out = writeByte_ & 0b10111111;
    if (data)
        out |= 0b01000000;
    SPIArduino::sendSync(out, outDev_);
}
} // namespace pinCfg
