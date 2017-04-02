#include <string.h>
#include <Wire.h>
#include <SPI.h>
#include <RFM69.h>

#define DEBUG

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


void RFM_Setup() {
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setPowerLevel(31);
  radio.encrypt(ENCRYPTKEY);
}

bool RFM_Transmit() {
  // Prepare message
  char buf[11];
  char radiopacket[64] = {0};

  // Temperature
  sprintf(buf, "%.5u", temperature);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Pressure
  sprintf(buf, "%.6lu", pressure);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Humidity
  sprintf(buf, "%0.4lu", humidity);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Lumosity
  sprintf(buf, "%.5u", lumosity);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  // Voltage
  sprintf(buf, "%.5u", voltage);
  strcat(radiopacket, buf);
  strcat(radiopacket, ";");

  #ifdef DEBUG
    Serial.print("Sending: ");
    Serial.println(radiopacket);
  #endif
  if (radio.sendWithRetry(RECEIVER, radiopacket, strlen(radiopacket))) {
    #ifdef DEBUG
      Serial.println("Data send and received!");
    #endif
  }
  radio.receiveDone();

  return true;
}

