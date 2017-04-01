#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

extern float temperature = 0;
extern float pressure = 0;
extern float humidity = 0;

void BME_Setup() {
  if(!bme.begin()) {
    Serial.println("BME280 not found");
  }
}

void BME_Read() {
  temperature = bme.readTemperature();
  pressure = bme.readPressure();
  humidity = bme.readHumidity();
}
