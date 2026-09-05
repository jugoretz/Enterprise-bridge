//flashes three LEDs, with two chosen at random lit at any given time.

#define LED1 35
#define LED2 36
#define LED3 37

unsigned long previousMillis = 0;
const long interval = 1000; // 1 second
int leds[] = {LED1, LED2, LED3};
int lastFirst = -1, lastSecond = -1;

void setup() {
  randomSeed(analogRead(0)); // Seed for randomness
  for (int i = 0; i < 3; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Turn off previous LEDs
    if (lastFirst != -1 && lastSecond != -1) {
      digitalWrite(leds[lastFirst], LOW);
      digitalWrite(leds[lastSecond], LOW);
    }

    // Pick two different random LEDs
    int first = random(0, 3);
    int second;
    do {
      second = random(0, 3);
    } while (second == first);

    digitalWrite(leds[first], HIGH);
    digitalWrite(leds[second], HIGH);

    // Store the last two for turning off next cycle
    lastFirst = first;
    lastSecond = second;
  }
}