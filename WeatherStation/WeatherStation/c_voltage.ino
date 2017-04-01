#define PIN_VOLTAGE   A1
extern float voltage = 0;

void voltage_Setup() {
  
}

float voltage_Read() {
  voltage = analogRead(PIN_VOLTAGE)*3.3/1024;
}
