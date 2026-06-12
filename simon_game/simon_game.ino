const int BUZZER_PIN = 10;

void setup() {
}

void loop() {
  tone(BUZZER_PIN, 2000);
  delay(500);

  noTone(BUZZER_PIN);
  delay(500);
}
