const int LED_RIGHT = 2;
const int LED_DOWN  = 3;
const int LED_LEFT  = 4;
const int LED_UP    = 5;

void setup() {
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(LED_DOWN, OUTPUT);
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_UP, OUTPUT);
}

void loop() {
  digitalWrite(LED_RIGHT, HIGH);
  delay(300);
  digitalWrite(LED_RIGHT, LOW);

  digitalWrite(LED_DOWN, HIGH);
  delay(300);
  digitalWrite(LED_DOWN, LOW);

  digitalWrite(LED_LEFT, HIGH);
  delay(300);
  digitalWrite(LED_LEFT, LOW);

  digitalWrite(LED_UP, HIGH);
  delay(300);
  digitalWrite(LED_UP, LOW);

  delay(500);
}
