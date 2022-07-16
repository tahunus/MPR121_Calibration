#ifndef MPR121_H
  #define MPR121_H
  #define I2CADDR_DEFAULT 0x5A 

  #include "globalConst.h"
  #include "Arduino.h"
  #include <Adafruit_BusIO_Register.h>
  #include <Adafruit_I2CDevice.h>

  class MPR121 {
    public:
      MPR121();  //Hardware I2C
      bool begin(uint8_t i2caddr = I2CADDR_DEFAULT, TwoWire *theWire = &Wire);
      uint8_t readRegister8(uint8_t reg);
      void writeRegister(uint8_t reg, uint8_t value);
      void setThresholds(uint8_t touch, uint8_t release);

    private:
      Adafruit_I2CDevice *i2c_dev = NULL;
  };

// Unused functions of original MPR121
/*    uint16_t readRegister16(uint8_t reg);      
      uint16_t touched(void);
      uint16_t filteredData(uint8_t t);
      uint16_t baselineData(uint8_t t);
*/

#endif
