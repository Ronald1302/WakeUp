#include <string.h>
#include <Wire.h>
#include <SPI.h>
#include <RFM69.h>

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
  char radiopacket[64] = {0};
  sprintf(radiopacket, "%0.5u %0.6lu %0.4lu %0.5u %0.5u", temperature, pressure, humidity, lumosity, voltage);

  Serial.print("Sending: ");
  Serial.println(radiopacket);
  if (radio.sendWithRetry(RECEIVER, radiopacket, strlen(radiopacket))) {
      Serial.println("Data send and received!");
  }
  radio.receiveDone();

  return true;
}

