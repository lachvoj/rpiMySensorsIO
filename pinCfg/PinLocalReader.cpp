#include <Arduino.h>

#include "PinLocalReader.hpp"

namespace pinCfg
{
PinLocalReader::PinLocalReader(uint8_t inPin) : inPin_(inPin)
{
    pinMode(inPin_, BCM2835_GPIO_FSEL_INPT);
}

bool PinLocalReader::readPin()
{
    return (bool)digitalRead(inPin_);
}
} // namespace pinCfg
