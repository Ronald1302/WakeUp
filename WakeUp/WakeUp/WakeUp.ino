#include <Arduino.h>
#include <U8x8lib.h>
#include <RFM69.h>
#include <SPI.h>
#include <DS3231.h>
#include <SD.h>

#define SERIAL_BAUD   115200

// Setup radio
#define NETWORKID     100
#define NODEID        1  
#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY     "sampleEncryptKey"
#define IS_RFM69HW    true
#define RFM69_CS      10
#define RFM69_IRQ     2
#define RFM69_IRQN    0
RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HW, RFM69_IRQN);

// Setup SD
#define SD_CS         9

// OLED
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);         

// RTC
DS3231  rtc(SDA, SCL);

void setup() {
  // Setup Serial
  while (!Serial);
  Serial.begin(SERIAL_BAUD);
  Serial.println("RFM69H Receiver");

  // setup RTC
  rtc.begin();

  // Setup OLED
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Setup radio
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
  radio.encrypt(ENCRYPTKEY);

  // Setup SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
  }

  Serial.println("Date;Time;Temperature;Pressure;Lumosity;Humidity;Temperature;Voltage;");
}

void loop() {
  if (radio.receiveDone()) {
      if (radio.ACKRequested()) {
        radio.sendACK();
      }


      // Setup vars
      char temp[64] = {0};
      char buf[10];

      // Date
      strcat(temp, rtc.getDateStr());
      strcat(temp, ";");

      // Time
      strcat(temp, rtc.getTimeStr());
      strcat(temp, ";");
      
      // Signal strength
      int valueI = radio.RSSI;
      itoa(valueI, buf, 10);
      strcat(temp, buf);
      strcat(temp, ";");

      // Combine data
      strcat(temp, radio.DATA);
      Serial.println(temp);
      
      // Show on OLED
      u8x8.drawString(0,1,(char*)rtc.getTimeStr());
      u8x8.drawString(0,2,temp);
      u8x8.drawString(0,4,buf);
      u8x8.refreshDisplay();

      // Save to SD
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      if (dataFile) {
        dataFile.println((char*)radio.DATA);
        dataFile.close();
      }
  }

  radio.receiveDone();
  Serial.flush();
}
