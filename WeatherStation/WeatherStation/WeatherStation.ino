#include <string.h>
#include <Wire.h>
#include <SPI.h>
#include <RFM69.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_TSL2561_U.h>
#include "DHT.h"

#define INTERVAL      1000

// FRM69
#define NETWORKID     100
#define NODEID        2
#define RECEIVER      1
#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define IS_RFM69HW    true
#define RFM69_CS      10
#define RFM69_IRQ     2
#define RFM69_IRQN    0
#define RFM69_RST     9
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HW, RFM69_IRQN);

// BMP280 temperature/pressure
Adafruit_BMP280 bme;

// TSL2561 Lumosity
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// DHT-22 temperature/humidity sensor
#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Voltage
#define PIN_VOLTAGE   A1


void setup() {
  while (!Serial);
  Serial.begin(115200);
  Serial.println("RFM69H Transmitter");

  // Init BMP280
  bme.begin();

  // Init TSL2561
  tsl.begin();
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);

  // Initialize radio
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setPowerLevel(31);
  radio.encrypt(ENCRYPTKEY);

  // Init DHT-22
  dht.begin();
  
  Serial.println("Temperature;Pressure;Lumosity;Humidity;Temperature;Voltage;");
}


void loop() {
  // Prepare strings to transmit
  float valueF;
  char buf[10];
  char radiopacket[64] = {0};

  // Temperature
  valueF = bme.readTemperature();
  dtostrf(valueF, 5, 2, buf);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");
  
  // Pressure
  valueF = bme.readPressure();
  dtostrf(valueF, 9, 2, buf);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Lumosity
  sensors_event_t event;
  tsl.getEvent(&event);
  valueF = event.light;
  dtostrf(valueF, 5, 0, buf);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Humidity
  valueF = dht.readHumidity();
  dtostrf(valueF, 5, 2, buf);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Temperature
  valueF = dht.readTemperature();
  dtostrf(valueF, 5, 2, buf);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Voltage (battery)
  valueF = analogRead(PIN_VOLTAGE)*3.3/1024;
  dtostrf(valueF, 4, 2, buf);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Transmit
  Serial.println(radiopacket);
  if (radio.sendWithRetry(RECEIVER, radiopacket, strlen(radiopacket))) {
    Serial.print("Send: ");
    Serial.println(radiopacket);
  }
  radio.receiveDone();
  Serial.flush();
  delay(INTERVAL);
}
