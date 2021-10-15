#include <Arduino.h>

#include "PinLocalReader.hpp"

namespace pinCfg
{
PinLocalReader::PinLocalReader(uint8_t inPin) : inPin_(inPin)
{
    pinMode(inPin_, BCM2835_GPIO_FSEL_INPT);
    digitalWrite(inPin_, HIGH); // enabling pullup
}

bool PinLocalReader::readPin()
{
    return static_cast<bool>(digitalRead(inPin_));
}
} // namespace pinCfg
