#include "MPR121.h"
extern uint8_t p[21]; //input variables coming from Blynk interface

MPR121::MPR121() {}  // Default constructor

bool MPR121::begin(uint8_t i2caddr, TwoWire *theWire) {
  if (i2c_dev) delete i2c_dev;
  i2c_dev = new Adafruit_I2CDevice(i2caddr, theWire);
  delay(10);
  if (!i2c_dev->begin()) {
    Serial.print(i2caddr,HEX); Serial.println(": I2C Fail"); 
    return false;
  }
  writeRegister(MPR121_SOFTRESET, 0x63); //reset all regsiters to 0x00 except 0x5C=0x10 & 0x5D=0x24
  writeRegister(MPR121_ECR, 0x0);  //go to STOP Mode
  delay(10);
  uint8_t c = readRegister8(MPR121_CONFIG2);
  if (c != 0x24) {
    Serial.print(i2caddr,HEX); Serial.println(": Reset Fail");
    return false;
  }

  setThresholds(p[TT],p[RT]);
  writeRegister(MPR121_MHDR, p[MHDR]);
  writeRegister(MPR121_NHDR, p[NHDR]);
  writeRegister(MPR121_NCLR, p[NCLR]);
  writeRegister(MPR121_FDLR, p[FDLR]);
  writeRegister(MPR121_MHDF, p[MHDF]);
  writeRegister(MPR121_NHDF, p[NHDF]);
  writeRegister(MPR121_NCLF, p[NCLF]);
  writeRegister(MPR121_FDLF, p[FDLF]);
  writeRegister(MPR121_NHDT, p[NHDT]);
  writeRegister(MPR121_NCLT, p[NCLT]);
  writeRegister(MPR121_FDLT, p[FDLT]);
  writeRegister(MPR121_DEBOUNCE, (p[DR] << 4) + p[DT]);

  uint8_t AFE = 1;  // CDC=1 as default. CDCx will be used instead
  switch (p[FFI]) {
    case 10: AFE += 64; break;
    case 18: AFE += 128; break;
    case 34: AFE += 192; break;
  }
  uint8_t FCR = 32; // CDT=0.5uS (32 BIN) as default. CDTx will be used instead
  switch (p[SFI]) {
    case 6: FCR += 8; break;
    case 10: FCR += 16; break;
    case 18: FCR += 24; break;
  }
  FCR += log10(p[ESI]) / log10(2);
  writeRegister(MPR121_CONFIG1, AFE); 
  writeRegister(MPR121_CONFIG2, FCR); 

  writeRegister(MPR121_UPLIMIT, p[USL]);     
  writeRegister(MPR121_TARGETLIMIT, p[TL]); 
  writeRegister(MPR121_LOWLIMIT, p[LSL]);   

  writeRegister(MPR121_AUTOCONFIG0, AFE - 1 + 27); // Bxx011011
                //AFE-1 = FFI when CDT = 1 (default). Must match FFI in 0x5C
                //RETRY=01 (2 times) for autoconfig and autoreconfig
                //BVA=10 same as the CL bits in ECR (0x5E)
                //ARE=1 (auto reconfig enabled), ACE=1 (autoconfig enabled)

  byte ECR_SETTING = B10000000 + 12; //5 bits baseline tracking, proximity disabled + 12 electrodes running
  writeRegister(MPR121_ECR, ECR_SETTING); 
  delay(80); //make time for autoconfig and possible auto reconfig of all electrodes
  
  for (uint8_t i = 0x5F; i < 0x72; i++) {  //Display results of autoconfig
    uint8_t r = readRegister8(i);
    if (i < 0x6B) {
      Serial.print("CDC"); Serial.print(i-95); 
      Serial.print(" ");Serial.print(i,HEX);
      Serial.print(": 0x"); Serial.print(r,HEX);
      Serial.print(" "); Serial.println(r,DEC);
    }
    else {
      if (i > 0x6B) {
        Serial.print("CDT"); Serial.print((i-108)*2);
        Serial.print(" ");Serial.print(i,HEX);
        Serial.print(": 0x"); Serial.print((r & B00000111),HEX); //lower 3 bits
        Serial.print(" "); Serial.println((r & B00000111),DEC);
        Serial.print("CDT"); Serial.print((i-108)*2+1);
        Serial.print(" ");Serial.print(i,HEX);
        Serial.print(": 0x"); Serial.print((r >> 4),HEX);  //upper 4 bits 
        Serial.print(" "); Serial.println((r >> 4),DEC);
      }
    }
  }
  return true;
}

void MPR121::setThresholds(uint8_t touch, uint8_t release) {  
  for (uint8_t i = 0; i < 12; i++) {  //set all thresholds to same value
    writeRegister(MPR121_TOUCHTH_0 + 2 * i, touch);
    writeRegister(MPR121_RELEASETH_0 + 2 * i, release);
  }
}

uint8_t MPR121::readRegister8(uint8_t reg) {
  Adafruit_BusIO_Register thereg = Adafruit_BusIO_Register(i2c_dev, reg, 1);
  return (thereg.read());
}

void MPR121::writeRegister(uint8_t reg, uint8_t value) {
  bool stop_required = true;  //MPR121 must be put in Stop Mode to write to most registers
  
  Adafruit_BusIO_Register ecr_reg = Adafruit_BusIO_Register(i2c_dev, MPR121_ECR, 1);
  uint8_t ecr_backup = ecr_reg.read();  //get current value of MPR121_ECR register

  if ((reg == MPR121_ECR) || ((0x73 <= reg) && (reg <= 0x7A))) stop_required = false;
  
  if (stop_required) ecr_reg.write(0x00);  //set to zero to set board in stop mode
  
  Adafruit_BusIO_Register the_reg = Adafruit_BusIO_Register(i2c_dev, reg, 1);
  the_reg.write(value);  //write value in passed register

  if (stop_required) ecr_reg.write(ecr_backup);  //write back the previous set ECR settings
}

// Unused functions of original MPR121
/*
uint16_t MPR121::filteredData(uint8_t t) {
  if (t > 12) return 0;
  return readRegister16(MPR121_FILTDATA_0L + t * 2);
}

uint16_t MPR121::baselineData(uint8_t t) {
  if (t > 12) return 0;
  uint16_t bl = readRegister8(MPR121_BASELINE_0 + t);
  return (bl << 2);
}

uint16_t MPR121::touched(void) {
  uint16_t t = readRegister16(MPR121_TOUCHSTATUS_L);
  return t & 0x0FFF;
}

uint16_t MPR121::readRegister16(uint8_t reg) {
  Adafruit_BusIO_Register thereg = Adafruit_BusIO_Register(i2c_dev, reg, 2, LSBFIRST);
  return (thereg.read());
}*/
