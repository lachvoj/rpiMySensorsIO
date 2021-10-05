#include "PinSPIReader.hpp"
#include "SPIArduino.hpp"

namespace pinCfg
{
PinSPIReader::PinSPIReader(uint8_t inPin, uint8_t inDev) : inDev_(inDev), readByte_(0b00000000 | (0x0F & inPin))
{
}

bool PinSPIReader::readPin()
{
    return (bool)SPIArduino::sendSync(readByte_, inDev_);
}
} // namespace pinCfg
