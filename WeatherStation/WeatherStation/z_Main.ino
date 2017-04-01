#include <string.h>
#include <Wire.h>

#define INTERVAL      1000


void setup() {
  while (!Serial);
  Serial.begin(115200);
  Serial.println("RFM69H Transmitter");

  BME_Setup();
  TSL_Setup();
  voltage_Setup();
  RFM_Setup();

  Serial.println("Temperature;Pressure;Humidity;Lumosity;Voltage;");
  delay(INTERVAL);
}


void loop() {
  // Read data
  BME_Read();
  TSL_Read();
  voltage_Read();

  // Sent data
  RFM_Transmit();

  // Finish loop
  Serial.flush();
  delay(INTERVAL);
}
