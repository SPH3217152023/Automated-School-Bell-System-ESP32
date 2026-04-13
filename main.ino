#include <Arduino.h>

// ===== Sensor pin =====
#define VIBRATION_SENSOR 4

// ===== System state =====
String mode = "Idle";

void setup() {
  Serial.begin(115200);
  pinMode(VIBRATION_SENSOR, INPUT);

  Serial.println("Automated Bell System Started");
}

void loop() {
  int sensorState = digitalRead(VIBRATION_SENSOR);

  if (sensorState == HIGH) {
    mode = "RINGING";
    Serial.println("Bell Triggered!");

    delay(2000); // simulate bell duration
    mode = "Idle";
  }

  delay(100);
}
