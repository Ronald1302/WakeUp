/* =========================== Includes ============================ */
#include <Wire.h>

/* =========================== Defines ============================ */
#define TSL_ADDRESS       0x39
#define TSL_CONFIG        0x01  // 1x gain, 101ms meas. time
#define TSL_T_MEAS        150   // Measurement time in ms (max)

#define TSL2561_LUX_K1T   (0x0040)  // 0.125 * 2^RATIO_SCALE
#define TSL2561_LUX_B1T   (0x01f2)  // 0.0304 * 2^LUX_SCALE
#define TSL2561_LUX_M1T   (0x01be)  // 0.0272 * 2^LUX_SCALE
#define TSL2561_LUX_K2T   (0x0080)  // 0.250 * 2^RATIO_SCALE
#define TSL2561_LUX_B2T   (0x0214)  // 0.0325 * 2^LUX_SCALE
#define TSL2561_LUX_M2T   (0x02d1)  // 0.0440 * 2^LUX_SCALE
#define TSL2561_LUX_K3T   (0x00c0)  // 0.375 * 2^RATIO_SCALE
#define TSL2561_LUX_B3T   (0x023f)  // 0.0351 * 2^LUX_SCALE
#define TSL2561_LUX_M3T   (0x037b)  // 0.0544 * 2^LUX_SCALE
#define TSL2561_LUX_K4T   (0x0100)  // 0.50 * 2^RATIO_SCALE
#define TSL2561_LUX_B4T   (0x0270)  // 0.0381 * 2^LUX_SCALE
#define TSL2561_LUX_M4T   (0x03fe)  // 0.0624 * 2^LUX_SCALE
#define TSL2561_LUX_K5T   (0x0138)  // 0.61 * 2^RATIO_SCALE
#define TSL2561_LUX_B5T   (0x016f)  // 0.0224 * 2^LUX_SCALE
#define TSL2561_LUX_M5T   (0x01fc)  // 0.0310 * 2^LUX_SCALE
#define TSL2561_LUX_K6T   (0x019a)  // 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6T   (0x00d2)  // 0.0128 * 2^LUX_SCALE
#define TSL2561_LUX_M6T   (0x00fb)  // 0.0153 * 2^LUX_SCALE
#define TSL2561_LUX_K7T   (0x029a)  // 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7T   (0x0018)  // 0.00146 * 2^LUX_SCALE
#define TSL2561_LUX_M7T   (0x0012)  // 0.00112 * 2^LUX_SCALE
#define TSL2561_LUX_K8T   (0x029a)  // 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8T   (0x0000)  // 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8T   (0x0000)  // 0.000 * 2^LUX_SCALE

/* =========================== External variables ============================ */
extern uint16_t lumosity = 0;

/* =========================== Start ============================ */
void TSL_Start() {
  /*
   * A function to start a single measurement
   * Enables the sensor, sets the configuration and starts a measurement
   */
  // start sensor
  Wire.beginTransmission(TSL_ADDRESS);
  Wire.write(0x80); Wire.write(0x03); // Enable sensor
  Wire.endTransmission();

  // Set measurement options
  Wire.beginTransmission(TSL_ADDRESS);
  Wire.write(0x81); Wire.write(TSL_CONFIG);
  Wire.endTransmission();
}

/* =========================== Read ============================ */
void TSL_Read() {
  /*
   * A function to collect the measurement data.
   * After collection the lumosity is calculated using the formula given in the datasheet
   * Finally, the sensor is turned off to save current
   */

  // Request the data from channel 0 (broad spectrum)
  Wire.beginTransmission(TSL_ADDRESS);
  Wire.write(0xAC);
  Wire.endTransmission();
  Wire.requestFrom(TSL_ADDRESS, 2);
  uint32_t channel0 = Wire.read() | (Wire.read() << 8);

  // Request the data from channel 1 (IR)
  Wire.beginTransmission(TSL_ADDRESS);
  Wire.write(0xAE);
  Wire.endTransmission();
  Wire.requestFrom(TSL_ADDRESS, 2);
  uint32_t channel1 = Wire.read() | (Wire.read() << 8);

  // shutdown sensor
  Wire.beginTransmission(TSL_ADDRESS);
  Wire.write(0x80); Wire.write(0x00);
  Wire.endTransmission();

  // Calculate lux
  uint32_t temp = (uint32_t)0x0FE7<<4;
  channel0 = (channel0 * temp) >> 10;
  channel1 = (channel1 * temp) >> 10;

  /* Find the ratio of the channel values (Channel1/Channel0) */
  temp = (channel1 << (10)) / channel0;
  temp = (temp + 1) >> 1;

  /* Use the ratio to perform the correct calculation */
  if ((temp >= 0) && (temp <= TSL2561_LUX_K1T)) {
    temp = ((channel0 * TSL2561_LUX_B1T) - (channel1 * TSL2561_LUX_M1T));
  } else if (temp <= TSL2561_LUX_K2T) {
    temp = ((channel0 * TSL2561_LUX_B2T) - (channel1 * TSL2561_LUX_M3T));
  } else if (temp <= TSL2561_LUX_K3T) {
    temp = ((channel0 * TSL2561_LUX_B3T) - (channel1 * TSL2561_LUX_M3T));
  } else if (temp <= TSL2561_LUX_K4T) {
    temp = ((channel0 * TSL2561_LUX_B4T) - (channel1 * TSL2561_LUX_M4T));
  } else if (temp <= TSL2561_LUX_K5T) {
    temp = ((channel0 * TSL2561_LUX_B5T) - (channel1 * TSL2561_LUX_M5T));
  } else if (temp <= TSL2561_LUX_K6T) {
    temp = ((channel0 * TSL2561_LUX_B6T) - (channel1 * TSL2561_LUX_M6T));
  } else if (temp <= TSL2561_LUX_K7T) {
    temp = ((channel0 * TSL2561_LUX_B7T) - (channel1 * TSL2561_LUX_M7T));
  } else if (temp > TSL2561_LUX_K8T) {
    temp = ((channel0 * TSL2561_LUX_B8T) - (channel1 * TSL2561_LUX_M8T));
  }

  /* Correct for negative values and obtain the absolute lumosity */
  if(temp < 0) temp = 0;
  temp += (1 << (13));
  lumosity = temp >> 14;
}
