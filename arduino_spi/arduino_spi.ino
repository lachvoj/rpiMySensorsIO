#include <SPI.h>

#define DEBUG 1

// track if digital pins are set as input or output (saves reading registers)
// Pins 0 to 9 can be used, pin 10 can be used for SS (but can be used if that's not required)
bool pin_output[11];

void setup()
{
    // enable console log
#ifdef DEBUG
    Serial.begin(115200);
    Serial.print("Serial setup finished.\n");
#endif

    // by default all pins are inputs
    for (int i = 0; i < 11; i++)
    {
        pin_output[i] = false;
    }

    // Set the Main in Secondary Out as an output
    pinMode(MISO, OUTPUT);
    // turn on SPI as a secondary
    // Set appropriate bit in SPI Control Register
    SPCR |= _BV(SPE);
}

void loop()
{
    byte in_byte;
    // SPIF indicates transmission complete (byte received)
    if ((SPSR & (1 << SPIF)) != 0)
    {
        in_byte = SPDR;
#ifdef DEBUG
        Serial.println(SPDR, HEX);
#endif
        // if no action bit sent then return same
        if (in_byte & 0x80)
            SPDR = in_byte;
        // if write set
        else if (in_byte & 0x20)
        {
            // set digital pin output - use mask to extract pin and output
            if (set_digital_pin(in_byte & 0x0F, in_byte & 0x40) != 0xFF)
                SPDR = in_byte;
        }
        // If value is only pin numbers then digital read
        else if (in_byte < 0x10)
        {
            byte return_val = read_digital_pin(in_byte);
            SPDR = return_val;
        }
        else if ((in_byte & 0x10) && !(in_byte & 0x20))
        {
            byte return_val = read_analog_pin(in_byte & 0x0F);
            SPDR = return_val;
        }
        else // Otherwise an error - return 0xff
        {
            SPDR = 0xFF;
        }
    }
}

byte set_digital_pin(byte address, bool high_value)
{
    if (address < 0 || address > 10)
        return 0xFF;
    // Is it currently set as output - if not then set to output
    if (!pin_output[address])
    {
        pinMode(address, OUTPUT);
        pin_output[address] = true;
    }
    // set to low if 0, otherwise 1
    if (high_value == 0)
        digitalWrite(address, LOW);
    else
        digitalWrite(address, HIGH);
}

// Returns 1 for high and 0 for low
// If invalid then returns 0xff
byte read_digital_pin(byte address)
{
    if (address < 0 || address > 10)
        return 0xFF;
    // if currently output then change to input
    if (pin_output[address])
    {
        pinMode(address, INPUT);
        pin_output[address] = false;
    }

    return digitalRead(address);
}

// As full byte is used for value there is no invalid value
// If invalid then returns 0x00
byte read_analog_pin(byte address)
{
    if (address < 0 || address > 5)
        return 0x00;
    int analog_val = analogRead(address);
    // Analog value is between 0 and 1024 - div by 4 for byte
    return (analog_val / 4);
}
