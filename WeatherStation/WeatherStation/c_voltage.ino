/* =========================== Defines ============================ */
#define PIN_VOLTAGE   A1
#define VOLTAGE_CORR  47373 // Vdd/ch/(R2/(R1+R2))

/* =========================== External variables ============================ */
extern uint16_t voltage = 0;

/* =========================== Read ============================ */
float voltage_Read() {
  float temp = analogRead(PIN_VOLTAGE);
  temp = temp * 47373 / 1000;
  voltage = (uint16_t)temp;
}
