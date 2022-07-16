#include "Credentials.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "Wire.h"
#include "MPR121.h"
#ifndef _BV
  #define _BV(bit) (1 << (bit)) 
#endif
const uint8_t n = 3; //number of breakout boards
MPR121 S[n] = {MPR121(),MPR121(),MPR121()};
int addr[n] = {0x5A,0x5C,0x5D}; //I2C addresses for n MPR121 breakout boards 

uint8_t p[21], START, oType; //input variables coming from Blynk interface
int c[n][42]; //register data from 3 breakout boards incl registers from 00x00 to 00x2A

int i,j; 
bool sensorStarted = false;
uint16_t oldT[n] = {0};
uint16_t newT[n] = {0};

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
}
 
void loop() {
  Blynk.run();
  if (START == 1) {
    Blynk.virtualWrite(V21,0); START = 0; //reset START button in Blynk Console
    if (!sensorStarted) Wire.begin (22,20); 
    for (i=0;i<n;i++) { //configure and start the 3 MPR121 breakout boards
      if (!S[i].begin(addr[i])) {Serial.println(String(addr[i]) + " Not Ok"); while (1); }
      oldT[i] = 0;
    }
    sensorStarted = true;
  }
  if (sensorStarted) runOutput();
}

//--------------------FUNCTIONS-----------------------------------
int baselineData (int i,int j) {  //baselineData of breakout board [i], electrode [j]
  return c[i][0x1E + j] << 2; 
}

int filteredData (int i, int j) { //filteredData of breakout board [i], electrode [j]
  return c[i][0x04 + j*2] + (c[i][0x04 + j*2 + 1] << 8); 
}

void printTouchData (String label) {
  Serial.print(label + String(addr[i]).substring(4) + String(j) + ", ");
  Serial.print("B:" + String(baselineData(i,j)));
  Serial.print(" F:" + String(filteredData(i,j)));
  Serial.println(" d:" + String(baselineData(i,j) - filteredData(i,j)));
}

void runOutput() {
  for (i=0;i<n;i++) {  //read touch, filtered & baseline data @ same time stamp of breakout boards 
    Wire.requestFrom(addr[0],42);  //registers from 00x00 to 00x2A
    j=0;
    while (Wire.available()) {
      c[i][j] = Wire.read();
      ++j;
    }
  }
  if (oType == 0) {//touched-released status of electrode[j] in board[i]
    for (i=0;i<n;i++) {
      for (j=0;j<12;j++) {
        newT[i] = c[i][j] + (c[i][j+1] << 8); 
        if ((newT[i] & _BV(j)) && !(oldT[i] & _BV(j))) printTouchData("Touched");
        if (!(newT[i] & _BV(j)) && (oldT[i] & _BV(j))) printTouchData("Released");
      }
      oldT[i] = newT[i];
    }
  }
  else if (oType > 0 && oType < 4) { //filtererdData of S0, S1 or S2
    for (j=0;j<12;j++) {
      Serial.print(filteredData(oType - 1,j)); Serial.print(",");}
    Serial.println(" ");
  }
  else if (oType > 3 && oType < 7) { //baselineData of S0, S1 or S2
    for (j=0;j<12;j++) {
      Serial.print(baselineData(oType - 4,j)); Serial.print(",");}
    Serial.println(" ");
  }
  else if (oType > 6 && oType < 10) { //baselineData & filteredData of S0, S1 or S2  
    for (j=0;j<12;j++) {
      Serial.print(baselineData(oType - 7,j)); Serial.print(",");
      Serial.print(filteredData(oType - 1,j)); Serial.print(","); }
    Serial.println(" ");
  }
  else if (oType == 10)  { //filteredData of S1, S2 & S3
    for (i=0;i<n;i++) {
      for (j=0;j<12;j++) {
        Serial.print(filteredData(i,j)); Serial.print(","); }
    Serial.println(" "); }
  }
  else if (oType == 11)  { //baselineData of S1, S2 & S3
    for (i=0;i<n;i++) {
      for (j=0;j<12;j++) {
        Serial.print(baselineData(i,j)); Serial.print(","); }
    Serial.println(" "); } 
  }
}  

//----BLYNK Functions to Input Data---------------------
BLYNK_WRITE(V0)  {p[USL] = param.asInt();}
BLYNK_WRITE(V1)  {p[LSL] = param.asInt();}
BLYNK_WRITE(V2)  {p[TL] = param.asInt();}
BLYNK_WRITE(V3)  {p[TT] = param.asInt();}
BLYNK_WRITE(V4)  {p[RT] = param.asInt();}
BLYNK_WRITE(V5)  {p[MHDR] = param.asInt();}
BLYNK_WRITE(V6)  {p[NHDR] = param.asInt();}
BLYNK_WRITE(V7)  {p[NCLR] = param.asInt();}
BLYNK_WRITE(V8)  {p[FDLR] = param.asInt();}
BLYNK_WRITE(V9)  {p[MHDF] = param.asInt();}
BLYNK_WRITE(V10)  {p[NHDF] = param.asInt();}
BLYNK_WRITE(V11)  {p[NCLF] = param.asInt();}
BLYNK_WRITE(V12)  {p[FDLF] = param.asInt();}
BLYNK_WRITE(V13)  {p[NHDT] = param.asInt();}
BLYNK_WRITE(V14)  {p[NCLT] = param.asInt();}
BLYNK_WRITE(V15)  {p[FDLT] = param.asInt();}
BLYNK_WRITE(V16)  {p[DT] = param.asInt();}
BLYNK_WRITE(V17)  {p[DR] = param.asInt();}
BLYNK_WRITE(V18)  {p[FFI] = param.asInt();}
BLYNK_WRITE(V19)  {p[SFI] = param.asInt();}
BLYNK_WRITE(V20)  {p[ESI] = param.asInt();}
BLYNK_WRITE(V21)  {START = param.asInt();}
BLYNK_WRITE(V22)  {oType = param.asInt();}

BLYNK_CONNECTED() {Blynk.syncAll();}  
