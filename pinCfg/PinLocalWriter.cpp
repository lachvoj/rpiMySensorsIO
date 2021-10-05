#include <Arduino.h>

#include "PinLocalWriter.hpp"

namespace pinCfg
{
PinLocalWriter::PinLocalWriter(uint8_t outPin) : outPin_(outPin)
{
    pinMode(outPin_, BCM2835_GPIO_FSEL_OUTP);
}

void PinLocalWriter::writePin(bool data)
{
    digitalWrite(outPin_, static_cast<uint8_t>(data));
}
} // namespace pinCfg
