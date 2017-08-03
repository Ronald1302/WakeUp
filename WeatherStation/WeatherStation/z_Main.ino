#define INTERVAL      1000

void setup() {
  while (!Serial);
  Serial.begin(115200);
  Serial.println("WeatherStation");

  BME_Setup();
  RFM_Setup();

  Serial.println("Temperature;Pressure;Humidity;Lumosity;Voltage;");
  delay(INTERVAL);
}


void loop() {
  // Start measurement aquisition
  BME_Start();
  TSL_Start();
  voltage_Read();

  // Collect measurement data
  delay(BME_T_MEAS);
  BME_Read();

  delay(TSL_T_MEAS-BME_T_MEAS);
  TSL_Read();

  // Sent data
  RFM_Transmit();
  Serial.println("test");

  // Finish loop
  Serial.flush();
  delay(INTERVAL-BME_T_MEAS-TSL_T_MEAS);
}
