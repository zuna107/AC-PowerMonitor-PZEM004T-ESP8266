#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

#define RELAY_1 2  // Pin 2 untuk Relay 1
#define RELAY_2 3  // Pin 3 untuk Relay 2

SoftwareSerial pzemSerial(9, 8);  // Pin 9 sebagai TX, Pin 8 sebagai RX untuk PZEM
PZEM004Tv30 pzem1(pzemSerial);

float voltage1, current1, power1, energy1, frequency1, wh1 = 0.0;
bool relay1_was_active = false;
unsigned long relay2_start_time = 0;
bool relay2_is_active = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
}

void loop() {
  voltage1 = zeroIfNan(pzem1.voltage());
  current1 = zeroIfNan(pzem1.current());
  power1 = zeroIfNan(pzem1.power());
  energy1 = zeroIfNan(pzem1.energy() / 1000); 
  frequency1 = zeroIfNan(pzem1.frequency());

  wh1 += power1 * (2.0 / 3600.0);

  Serial.println("\n---------- DATA SENSOR ----------");
  Serial.print("Voltage      : "); Serial.print(voltage1); Serial.println(" V");
  Serial.print("Current      : "); Serial.print(current1); Serial.println(" A");
  Serial.print("Power        : "); Serial.print(power1); Serial.println(" W");
  Serial.print("Energy       : "); Serial.print(energy1); Serial.println(" kWh");
  Serial.print("Frequency    : "); Serial.print(frequency1); Serial.println(" Hz");
  Serial.print("Watt-hour    : "); Serial.print(wh1); Serial.println(" Wh");
  Serial.println("----------------------------------");

  if (current1 >= 1.0) {
    digitalWrite(RELAY_1, LOW); // Relay 1 AKTIF
    Serial.println("⚠️ Arus tinggi! Relay 1 AKTIF.");
    relay1_was_active = true;
  } else {
    digitalWrite(RELAY_1, HIGH); // Relay 1 NONAKTIF
    Serial.println("Relay 1 mati");
  }

  unsigned long currentMillis = millis();
  unsigned long startTime = 7 * 60 * 60 * 1000;
  unsigned long endTime = 7 * 60 * 60 * 1000 + 10 * 60 * 1000;

  if (relay1_was_active && currentMillis >= startTime && currentMillis <= endTime) {
    digitalWrite(RELAY_2, LOW); // Relay 2 AKTIF
    Serial.println("Relay 2 AKTIF");
  } else {
    digitalWrite(RELAY_2, HIGH); 
  }

  delay(2000);
}

// Fungsi untuk menangani nilai NaN
float zeroIfNan(float v) {
  return isnan(v) ? 0 : v;
}
