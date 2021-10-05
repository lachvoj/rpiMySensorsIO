#pragma once

using namespace std;

namespace pinCfg
{
class SPIArduino
{
  private:
    static SPIArduino* instance_;
    static uint8_t lastDev_;
  public:
    SPIArduino();
    ~SPIArduino();
    static uint8_t sendSync(uint8_t data, uint8_t dev = 0);
    // static void chipSelect(uint8_t cs);
};
} // namespace pinCfg
