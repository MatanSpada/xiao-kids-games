// XIAO Kids Game - Simon Memory Game
// Board: Seeeduino XIAO SAMD21

// ---------- LED pins ----------
const int LED_RIGHT = 2; // Red / Right
const int LED_DOWN  = 3; // Yellow / Down
const int LED_LEFT  = 4; // Green / Left
const int LED_UP    = 5; // Blue / Up

// ---------- Button pins ----------
const int BTN_YELLOW = 6;
const int BTN_BLUE   = 7;
const int BTN_GREEN  = 8;
const int BTN_RED    = 9;

// ---------- Buzzer ----------
const int BUZZER_PIN = 10;

// If using Act pin on buzzer module, keep false.
// If later you connect Psv instead of Act, change to true.
const bool BUZZER_USE_TONE = false;

// ---------- Game constants ----------
const int STEP_RIGHT = 0;
const int STEP_DOWN  = 1;
const int STEP_LEFT  = 2;
const int STEP_UP    = 3;

const int MAX_LEVEL = 16;

// Predefined sequence.
// Each level adds one more item from this sequence.
const int GAME_SEQUENCE[MAX_LEVEL] = {
  STEP_RIGHT,
  STEP_DOWN,
  STEP_LEFT,
  STEP_UP,
  STEP_RIGHT,
  STEP_LEFT,
  STEP_DOWN,
  STEP_UP,
  STEP_DOWN,
  STEP_RIGHT,
  STEP_UP,
  STEP_LEFT,
  STEP_RIGHT,
  STEP_DOWN,
  STEP_UP,
  STEP_LEFT
};

int currentLevel = 1;

// Reset handling
const unsigned long LONG_RED_PRESS_MS = 1200;
const unsigned long RED_MULTI_PRESS_WINDOW_MS = 3000;
const int RED_MULTI_PRESS_COUNT = 5;

int redPressCounter = 0;
unsigned long firstRedPressTime = 0;

// ---------- Helpers ----------

int ledForStep(int step) {
  switch (step) {
    case STEP_RIGHT: return LED_RIGHT;
    case STEP_DOWN:  return LED_DOWN;
    case STEP_LEFT:  return LED_LEFT;
    case STEP_UP:    return LED_UP;
  }
  return LED_RIGHT;
}

int buttonToStep() {
  if (digitalRead(BTN_RED) == LOW)    return STEP_RIGHT;
  if (digitalRead(BTN_YELLOW) == LOW) return STEP_DOWN;
  if (digitalRead(BTN_GREEN) == LOW)  return STEP_LEFT;
  if (digitalRead(BTN_BLUE) == LOW)   return STEP_UP;

  return -1;
}

bool isAnyButtonPressed() {
  return buttonToStep() != -1;
}

void allLedsOff() {
  digitalWrite(LED_RIGHT, LOW);
  digitalWrite(LED_DOWN, LOW);
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_UP, LOW);
}

void allLedsOn() {
  digitalWrite(LED_RIGHT, HIGH);
  digitalWrite(LED_DOWN, HIGH);
  digitalWrite(LED_LEFT, HIGH);
  digitalWrite(LED_UP, HIGH);
}

void flashAllLedsTwice() {
  for (int i = 0; i < 2; i++) {
    allLedsOn();
    delay(500);
    allLedsOff();
    delay(500);
  }
}

void beepError() {
  // No LED action here, as requested.

  if (BUZZER_USE_TONE) {
    tone(BUZZER_PIN, 200, 700);
    delay(750);
    noTone(BUZZER_PIN);
  } else {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(700);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(300);
}

void showOneStep(int step, int onMs = 450, int gapMs = 180) {
  int led = ledForStep(step);

  digitalWrite(led, HIGH);
  delay(onMs);
  digitalWrite(led, LOW);
  delay(gapMs);
}

void introAnimation() {
  allLedsOff();

  // 3 rotations before game starts.
  for (int round = 0; round < 3; round++) {
    showOneStep(STEP_RIGHT, 180, 80);
    showOneStep(STEP_DOWN, 180, 80);
    showOneStep(STEP_LEFT, 180, 80);
    showOneStep(STEP_UP, 180, 80);
  }

  delay(500);
}

void showCurrentSequence() {
  delay(400);

  for (int i = 0; i < currentLevel; i++) {
    showOneStep(GAME_SEQUENCE[i]);
  }

  delay(300);
}

void resetGame() {
  currentLevel = 1;
  redPressCounter = 0;
  firstRedPressTime = 0;

  allLedsOff();
  delay(300);
  introAnimation();
  showCurrentSequence();
}

bool registerRedTapAndCheckReset() {
  unsigned long now = millis();

  if (redPressCounter == 0 || (now - firstRedPressTime) > RED_MULTI_PRESS_WINDOW_MS) {
    redPressCounter = 1;
    firstRedPressTime = now;
  } else {
    redPressCounter++;
  }

  if (redPressCounter >= RED_MULTI_PRESS_COUNT) {
    return true;
  }

  return false;
}

bool checkLongRedPressReset() {
  if (digitalRead(BTN_RED) != LOW) {
    return false;
  }

  unsigned long start = millis();

  while (digitalRead(BTN_RED) == LOW) {
    if (millis() - start >= LONG_RED_PRESS_MS) {
      // Wait until release, so it does not immediately trigger again.
      while (digitalRead(BTN_RED) == LOW) {
        delay(10);
      }
      delay(150);
      return true;
    }

    delay(10);
  }

  return false;
}

int waitForButtonPressOrReset(bool &resetRequested) {
  resetRequested = false;

  while (true) {
    // Long red press reset
    if (digitalRead(BTN_RED) == LOW) {
      unsigned long start = millis();

      while (digitalRead(BTN_RED) == LOW) {
        if (millis() - start >= LONG_RED_PRESS_MS) {
          while (digitalRead(BTN_RED) == LOW) {
            delay(10);
          }
          delay(150);
          resetRequested = true;
          return -1;
        }

        delay(10);
      }

      // It was a short red press.
      delay(40); // debounce

      if (registerRedTapAndCheckReset()) {
        resetRequested = true;
        return -1;
      }

      // Red button means STEP_RIGHT.
      return STEP_RIGHT;
    }

    if (digitalRead(BTN_YELLOW) == LOW) {
      delay(40);
      while (digitalRead(BTN_YELLOW) == LOW) {
        delay(10);
      }
      delay(80);
      return STEP_DOWN;
    }

    if (digitalRead(BTN_GREEN) == LOW) {
      delay(40);
      while (digitalRead(BTN_GREEN) == LOW) {
        delay(10);
      }
      delay(80);
      return STEP_LEFT;
    }

    if (digitalRead(BTN_BLUE) == LOW) {
      delay(40);
      while (digitalRead(BTN_BLUE) == LOW) {
        delay(10);
      }
      delay(80);
      return STEP_UP;
    }

    delay(10);
  }
}

bool playUserInputForCurrentLevel() {
  for (int i = 0; i < currentLevel; i++) {
    bool resetRequested = false;
    int pressedStep = waitForButtonPressOrReset(resetRequested);

    if (resetRequested) {
      resetGame();
      return false;
    }

    int expectedStep = GAME_SEQUENCE[i];

    if (pressedStep != expectedStep) {
      beepError();
      showCurrentSequence();
      return false;
    }

    // Small visual feedback for correct button.
    showOneStep(pressedStep, 120, 80);
  }

  return true;
}

// ---------- Arduino lifecycle ----------

void setup() {
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(LED_DOWN, OUTPUT);
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_UP, OUTPUT);

  pinMode(BTN_YELLOW, INPUT_PULLUP);
  pinMode(BTN_BLUE, INPUT_PULLUP);
  pinMode(BTN_GREEN, INPUT_PULLUP);
  pinMode(BTN_RED, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  allLedsOff();

  introAnimation();
  showCurrentSequence();
}

void loop() {
  bool success = playUserInputForCurrentLevel();

  if (success) {
    flashAllLedsTwice();

    if (currentLevel < MAX_LEVEL) {
      currentLevel++;
    } else {
      // Finished all predefined levels.
      // Start again from level 1.
      currentLevel = 1;
    }

    showCurrentSequence();
  }
}
