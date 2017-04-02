#include <Wire.h>

/* ========= Defines ========== */
#define BME_ADDRESS     0x76 // I2C address of sensor
#define BME_SET_MEAS    0x25 // Forced mode, 1x oversampling (t&p)
#define BME_SET_HUM     0x01 // 1x oversampling
#define BME_SET_CONFIG  0x50 // Filter off, 0.5ms ts, I2C
#define BME_T_MEAS      15   // Measurement time in ms (max)

/* ========= External variables ========== */
extern int32_t temperature = 0;
extern uint32_t pressure = 0;
extern uint32_t humidity = 0;

/* ========= BME-related variables ========== */
uint8_t BME_CAL_H1,BME_CAL_H3,BME_CAL_H6;
uint16_t BME_CAL_T1, BME_CAL_P1;
int16_t BME_CAL_T2,BME_CAL_T3, BME_CAL_P2,BME_CAL_P3,BME_CAL_P4,BME_CAL_P5,BME_CAL_P6,BME_CAL_P7,BME_CAL_P8,BME_CAL_P9, BME_CAL_H2,BME_CAL_H4,BME_CAL_H5;

/* ========= Functions ========== */
void BME_Setup() {
  /*
   * Function to setup the BME sensor. Sets measurement settings and collects calibration data
   */
   
  // Setup sensor
  Wire.beginTransmission(BME_ADDRESS);
  Wire.write(0xF5); Wire.write(BME_SET_CONFIG);
  Wire.write(0xF2); Wire.write(BME_SET_HUM);
  Wire.write(0xF4); Wire.write(BME_SET_MEAS);
  Wire.endTransmission();

  // Collect calibration data
  Wire.beginTransmission(BME_ADDRESS);
  Wire.write(0x88);
  Wire.endTransmission();
  Wire.requestFrom(BME_ADDRESS, 26);

  // Process calibration data
  BME_CAL_T1 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_T2 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_T3 = (Wire.read() | (Wire.read() << 8));

  BME_CAL_P1 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P2 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P3 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P4 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P5 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P6 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P7 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P8 = (Wire.read() | (Wire.read() << 8));
  BME_CAL_P9 = (Wire.read() | (Wire.read() << 8));

  Wire.read();
  BME_CAL_H1 = Wire.read();

  // Collect calibration data
  Wire.beginTransmission(BME_ADDRESS);
  Wire.write(0xE1);
  Wire.endTransmission();
  Wire.requestFrom(BME_ADDRESS, 8);

  BME_CAL_H2 = Wire.read() | (Wire.read() << 8);
  BME_CAL_H3 = Wire.read();
  byte b0 = Wire.read();
  byte b1 = Wire.read();
  BME_CAL_H4 = (b0 << 4) | (0x0F & b1);
  b0 = Wire.read();
  BME_CAL_H5 = (b0 << 4) | ((b1 >> 4) & 0x0F);
  BME_CAL_H6 = Wire.read();
}

void BME_Start() {
  /*
   * Method to start a measurement with the BME sensor.
   * After start, wait for a minimal time (see datasheet) before collecting the data
   */
   
  // Start a single measurement
  Wire.beginTransmission(BME_ADDRESS);
  Wire.write(0xF4); Wire.write(BME_SET_MEAS);
  Wire.endTransmission();
}

void BME_Read() {
  /*
   * Function to retrieve and calculate the measured temperature, pressure and humidity.
   * Results are stored in global variables
   */
   
  // Collect data
  Wire.beginTransmission(BME_ADDRESS);
  Wire.write(0xF7);
  Wire.endTransmission();
  Wire.requestFrom(BME_ADDRESS, 8);
  
  // Read raw data
  uint32_t BME_data[8],up,ut,uh;
  uint8_t n = 0;
  while(Wire.available()){
      BME_data[n] = Wire.read();
      n += 1;
  }
  up = (BME_data[0] << 12) | (BME_data[1] << 4) | (BME_data[2] >> 4);
  ut = (BME_data[3] << 12) | (BME_data[4] << 4) | (BME_data[5] >> 4);
  uh = (BME_data[6] << 8) | BME_data[7];

  // Calculate temperature
  int32_t T_var1 = ((((ut>>3) - ((int32_t)BME_CAL_T1<<1))) * ((int32_t)BME_CAL_T2)) >> 11;
  int32_t T_var2 = (((((ut>>4) - ((int32_t)BME_CAL_T1)) * ((ut>>4) - ((int32_t)BME_CAL_T1))) >> 12) * ((int32_t)BME_CAL_T3)) >> 14;
  int32_t T_var3 = T_var1 + T_var2;
  temperature = (T_var3*5+128) >> 8;
  
  // Calculate pressure
  int64_t P_var3 = 0;
  int64_t P_var1 = (int64_t)T_var3 - 128000;
  int64_t P_var2 = P_var1 * P_var1 * (int64_t)BME_CAL_P6;
  P_var2 = P_var2 + ((P_var1 * (int64_t)BME_CAL_P5) << 17);
  P_var2 = P_var2 + (((int64_t)BME_CAL_P4) << 35);
  P_var1 = ((P_var1 * P_var1 * (int64_t)BME_CAL_P3) >> 8) + ((P_var1 * (int64_t)BME_CAL_P2) << 12);
  P_var1 = (((((int64_t)1) << 47) + P_var1)) * ((int64_t)BME_CAL_P1) >> 33;
  if (P_var1 != 0) {
    P_var3  = 1048576 - up;
    P_var3 = (((P_var3 << 31) - P_var2) * 3125)/P_var1;
    P_var1 = (((int64_t)BME_CAL_P9) * (P_var3 >> 13) * (P_var3 >> 13)) >> 25;
    P_var2 = (((int64_t)BME_CAL_P8) * P_var3) >> 19;
    P_var3 = ((P_var3 + P_var1 + P_var2) >> 8) + (((int64_t)BME_CAL_P7) << 4);
    pressure = (uint32_t)P_var3/256;
  }

  // Calculate humidity
  int32_t H_var1 = (T_var3 - ((int32_t)76800));
  H_var1 = (((((uh << 14) - (((int32_t)BME_CAL_H4) << 20) - (((int32_t)BME_CAL_H5) * H_var1)) +
    ((int32_t)16384)) >> 15) * (((((((H_var1 * ((int32_t)BME_CAL_H6)) >> 10) * (((H_var1 *
    ((int32_t)BME_CAL_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
    ((int32_t)BME_CAL_H2) + 8192) >> 14));
  H_var1 = (H_var1 - (((((H_var1 >> 15) * (H_var1 >> 15)) >> 7) * ((int32_t)BME_CAL_H1)) >> 4));
  H_var1 = (H_var1 < 0 ? 0 : H_var1);
  H_var1 = (H_var1 > 419430400 ? 419430400 : H_var1);
  humidity = (uint32_t)(H_var1>>12);
}


/*
 * Original code
 * 

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
}*/
