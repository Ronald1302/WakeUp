/* =========================== Defines ============================ */
#define PIN_VOLTAGE   A1

/* =========================== External variables ============================ */
extern float voltage = 0;

/* =========================== Read ============================ */
float voltage_Read() {
  voltage = analogRead(PIN_VOLTAGE)*3.3/1024;
}
