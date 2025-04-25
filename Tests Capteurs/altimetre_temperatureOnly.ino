#include <Wire.h>  // Include Wire library for I2C communication

#define MS5607_ADDRESS 0x76  // I2C address of MS5607 altimeter module

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  Wire.begin();

}
 
void loop() {
  // Request pressure measurement
  Wire.beginTransmission(MS5607_ADDRESS);
  Wire.write(0x48);  // Send command for D1 (pressure) conversion
  Wire.endTransmission();
  
  delay(30);  // Wait for conversion (adjust delay based on module specifications)
 
  // Read pressure measurement
  Wire.beginTransmission(MS5607_ADDRESS);
  Wire.write(0x00);  // Send command to read ADC result
  Wire.endTransmission(false);
 
  Wire.requestFrom(MS5607_ADDRESS, 3);  // Request 3 bytes of data
 

 
  // Request temperature measurement
  Wire.beginTransmission(MS5607_ADDRESS);
  Wire.write(0x58);  // Send command for D2 (temperature) conversion
  Wire.endTransmission();
 
  delay(30);  // Wait for conversion (adjust delay based on module specifications)
 
  // Read temperature measurement
  Wire.beginTransmission(MS5607_ADDRESS);
  Wire.write(0x00);  // Send command to read ADC result
  Wire.endTransmission(false);
 
  Wire.requestFrom(MS5607_ADDRESS, 3);  // Request 3 bytes of data
  byte temperatureData[3];
  if (Wire.available() >= 3) {
    temperatureData[0] = Wire.read();
    temperatureData[1] = Wire.read();
    temperatureData[2] = Wire.read();
  }
 
  // Calculate temperature value
  uint32_t D2 = ((uint32_t)temperatureData[0] << 16) | ((uint32_t)temperatureData[1] << 8) | temperatureData[2];
 
  // Calculate compensated temperature
  int32_t dT = D2 - (uint32_t)0x4000;
  int32_t TEMP = 2000 + dT * 5 / 131072;
  int64_t OFF = (uint32_t)7025 * (uint32_t)32768 + ((int64_t)dT * (uint32_t)3072) / (uint32_t)16;
  int64_t SENS = (uint32_t)1100 * (uint32_t)32768 + ((int64_t)dT * (uint32_t)2816) / (uint32_t)16;
  
 
  // Output pressure and temperature readings

  Serial.print("Temperature (°C): ");
  Serial.println(TEMP / 100.0);
 
  delay(1000);  // Delay before next reading
}
