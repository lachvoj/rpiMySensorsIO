#include <SPIDEV.h>

#include "SPIArduino.hpp"

namespace pinCfg
{
SPIArduino *SPIArduino::instance_ = nullptr;
uint8_t SPIArduino::lastDev_ = 0;

SPIArduino::SPIArduino()
{
    if (!instance_)
    {
        SPIDEVClass::begin();
        SPIDEVClass::setClockDivider(SPI_CLOCK_DIV16);
        instance_ = this;
    }
}

SPIArduino::~SPIArduino()
{
    if (instance_)
    {
        SPIDEVClass::end();
        instance_ = nullptr;
    }
}

uint8_t SPIArduino::sendSync(uint8_t data, uint8_t dev)
{
    if (!instance_)
        throw "Uninitialized SPI connection!";

    if (SPIArduino::lastDev_ != dev)
        SPIDEVClass::chipSelect(static_cast<int>(dev));

    uint8_t ret = SPIDEVClass::transfer(data);
    if (ret == 0xff)
        throw "SPI device returned error state.";

    return ret;
}
} // namespace pinCfg
