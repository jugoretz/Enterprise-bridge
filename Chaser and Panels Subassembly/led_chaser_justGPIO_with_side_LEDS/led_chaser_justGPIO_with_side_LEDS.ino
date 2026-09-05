#define NUM_LEDS 15  
int ledPins[NUM_LEDS] = {15, 7, 16, 17, 18, 38, 37, 36, 35, 0, 45, 48, 47, 21, 20};  

int flashLed1 = 10;  // First independent flashing LED
int flashLed2 = 11;  // Second independent flashing LED

unsigned long previousMillisChaser = 0;
const int chaserInterval = 100;  // Chaser speed

unsigned long previousMillisFlash1 = 0;
const int flashInterval1 = 1100;   // Flash rate for first LED

unsigned long previousMillisFlash2 = 0;
const int flashInterval2 = 1000;   // Flash rate for second LED

bool flashState1 = false;  // State of LED 1
bool flashState2 = false;  // State of LED 2

const int pauseTime = 500;  // Pause time with all LEDs off
bool isPaused = false;
unsigned long pauseStartMillis = 0;
bool lastLedDisplayed = false;

int chaserIndex = 0;  

void setup() {
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }
    pinMode(flashLed1, OUTPUT);
    pinMode(flashLed2, OUTPUT);
}

void loop() {
    unsigned long currentMillis = millis();

    // **Pause Mode: All LEDs Off**
    if (isPaused) {
        if (currentMillis - pauseStartMillis >= pauseTime) {
            isPaused = false;
            chaserIndex = 0;
            lastLedDisplayed = false;
        }
    } else {
        // **LED Chaser Effect**
        if (currentMillis - previousMillisChaser >= chaserInterval) {
            previousMillisChaser = currentMillis;
            
            for (int i = 0; i < NUM_LEDS; i++) {
                digitalWrite(ledPins[i], LOW);
            }

            if (chaserIndex < NUM_LEDS) {
                digitalWrite(ledPins[chaserIndex], HIGH);
                chaserIndex++;
            }

            if (chaserIndex >= NUM_LEDS && !lastLedDisplayed) {
                lastLedDisplayed = true;
            } else if (lastLedDisplayed) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    digitalWrite(ledPins[i], LOW);
                }
                isPaused = true;
                pauseStartMillis = millis();
            }
        }
    }

    // **Flashing LED 1**
    if (currentMillis - previousMillisFlash1 >= flashInterval1) {
        previousMillisFlash1 = currentMillis;
        flashState1 = !flashState1;
        digitalWrite(flashLed1, flashState1 ? HIGH : LOW);
    }

    // **Flashing LED 2**
    if (currentMillis - previousMillisFlash2 >= flashInterval2) {
        previousMillisFlash2 = currentMillis;
        flashState2 = !flashState2;
        digitalWrite(flashLed2, flashState2 ? HIGH : LOW);
    }
}