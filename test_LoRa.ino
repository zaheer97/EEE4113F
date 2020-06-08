#include <Adafruit_ADXL345_U.h>

// Arduino9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Arduino9x_TX
#include <SPI.h>
#include <RH_RF95.h>
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
// Blinky on receipt
#define LED 13
#include <Wire.h>  // Wire library - used for I2C communication
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
//float X_out, Y_out, Z_out;  // Outputs
int x,y,z,k; // for string purposes 
int i =0;
int counter =0 ;
char data[510];

void setup()
{
Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();

  delay(10);
pinMode(LED, OUTPUT);
pinMode(RFM95_RST, OUTPUT);
digitalWrite(RFM95_RST, HIGH);
while (!Serial);
//Serial.begin(9600);
//delay(100);
//Serial.println("Arduino LoRa RX Test!");
// manual reset
digitalWrite(RFM95_RST, LOW);
//delay(10);
digitalWrite(RFM95_RST, HIGH);
//delay(10);
while (!rf95.init()) {
Serial.println("LoRa radio init failed");
while (1);
}
//Serial.println("LoRa radio init OK!");
// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
if (!rf95.setFrequency(RF95_FREQ)) {
Serial.println("setFrequency failed");
while (1);
}
//Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
// The default transmitter power is 13dBm, using PA_BOOST.
// If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
// you can set transmitter powers from 5 to 23 dBm:
rf95.setTxPower(23, false);
}


void loop()
{
if (rf95.available())
{
// Should be a message for us now
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);
if (rf95.recv(buf, &len))
{
digitalWrite(LED, HIGH);

  x = (((int)buf[k+1]<<8)) | buf[k] ; 
  y = (((int)buf[k+3]<<8)) | buf[k+2];
  z = (((int)buf[k+5]<<8)) | buf[k+4];
  sprintf(data, " %d %d %d", x, y, z);
  Serial.println(data);

 counter =0 ;

 Wire.beginTransmission(ADXL345);
 Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
 Wire.endTransmission();
 Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
 x = ( Wire.read()| Wire.read() << 8); // X-axis value
 //X_out = X_out/32; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
 y = ( Wire.read()| Wire.read() << 8); // Y-axis value
 //Y_out = Y_out/32;
 z = ( Wire.read()| Wire.read() << 8); // Z-axis value
 //Z_out = Z_out/32;
 
digitalWrite(LED, LOW);
}
else
{
Serial.println("Receive failed");
}
}
}
