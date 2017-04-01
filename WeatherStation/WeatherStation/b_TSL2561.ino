#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

extern float lumosity = 0;

void TSL_Setup() {
  if(!tsl.begin()) {
    Serial.println("TSL2561 not found");
  }
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
}

void TSL_Read() {
  sensors_event_t event;
  tsl.getEvent(&event);
  lumosity = event.light;
}
