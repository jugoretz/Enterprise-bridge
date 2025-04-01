#include "Arduino.h"
#include "SD.h"
#include "SPI.h"
#include "AudioFileSourceSD.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "OneButton.h"

// SPI Setup for ESP32-C3
SPIClass spi = SPIClass(FSPI);
#define SD_CS      10  // to CS on SD module
#define SPI_MOSI   6   // to MOSI (or DI) on SD module
#define SPI_MISO   5   // to MISO (or DO) on SD module
#define SPI_SCK    4   // to CLK on SD module 

// I2S Audio Setup
#define I2S_DOUT   7  
#define I2S_BCLK   3  
#define I2S_LRC    2  

// Pushbutton and LED (Transistor Control)
#define ALERT_BUTTON  8   // Red Alert Trigger Button
#define LED_PIN       9   // LED or transistor control--Using GPIO 9 — confirmed safe on current ESP32-C3 board, but May not work on other modules if tied to SPI flash
#define BUTTON_LED_PIN 1   // Drives LED inside (or next to) the button

//onebutton functions
bool isMuted = false;

// OneButton object (true = active LOW)
OneButton button(ALERT_BUTTON, true);


extern AudioOutputI2S *out;
// Function prototypes
void handleAlertButton();

bool longPressTriggered = false;

void handlePress() {
  longPressTriggered = false;  // reset when button is pressed
}

void handleRelease() {
  if (!longPressTriggered) {
    handleAlertButton();  // only if it wasn't a long press
  }
}

void handleLongPress() {
  longPressTriggered = true;
  isMuted = !isMuted;
  if (out) {
    out->SetGain(isMuted ? 0.0 : 0.8);
  }
}


// Audio objects
AudioGeneratorWAV *wav;
AudioFileSourceSD *file;
AudioOutputI2S *out;

// Playback state tracking
enum PlaybackState {
    PLAY_OPENING,
    PLAY_ACTIVATE,
    LOOP_BRIDGE,
    PLAY_ALERT
};

PlaybackState currentState = PLAY_OPENING;
bool playingAlert = false;
int alertPlayCount = 0;

// Button Debounce
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // 50ms debounce

void setup() {
  Serial.end();  // Disable UART to free GPIO 1


    // Setup button and LED
    pinMode(ALERT_BUTTON, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUTTON_LED_PIN, LOW); // Start with button LED off
    pinMode(BUTTON_LED_PIN, OUTPUT);
    

    // Initialize SPI and SD Card
    spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
    if (!SD.begin(SD_CS, spi, 8000000)) {
        Serial.println("SD Card Mount Failed!");
        return;
    }
    Serial.println("SD Card Initialized.");

    // Initialize I2S
    out = new AudioOutputI2S();
    out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    out->SetGain(0.8);

    // Allocate WAV decoder
    wav = new AudioGeneratorWAV();

    // Start playback with opening.wav
    playFile("/opening.wav");

    //longpress setup
   button.attachPress(handlePress);
button.attachClick(handleRelease);         // fired on release
button.attachLongPressStart(handleLongPress);

button.setPressTicks(3000); //sets delay for long press to be considered long (in ms)
}

void loop() {
  button.tick();
    // Handle button press
    handleAlertButton();

    // Handle audio playback
    if (wav && wav->isRunning()) {
        wav->loop();

        // If in alert mode, blink the LED
        if (playingAlert) {
            blinkLED();
        }
    } else {
        Serial.println("Playback finished. Moving to next file...");

        if (playingAlert) {
            alertPlayCount++;
            if (alertPlayCount < 3) {
                // Replay red_alert.wav up to 3 times
                playFile("/red_alert.wav");
            } else {
                // Exit alert mode, turn off LED, resume looping bridge_looper.wav
                playingAlert = false;
                digitalWrite(LED_PIN, LOW);
                digitalWrite(BUTTON_LED_PIN, LOW);
                currentState = LOOP_BRIDGE;
                playFile("/bridge_looper.wav");
            }
        } else {
            // Normal playback sequence
            switch (currentState) {
                case PLAY_OPENING:
                    currentState = PLAY_ACTIVATE;
                    playFile("/bridge_activate.wav");
                    break;

                case PLAY_ACTIVATE:
                    currentState = LOOP_BRIDGE;
                    playFile("/bridge_looper.wav");
                    break;

                case LOOP_BRIDGE:
                    playFile("/bridge_looper.wav");  // Loop indefinitely
                    break;

                default:
                    break;
            }
        }
    }
}

// Function to play a specific file
void playFile(const char* filename) {
    Serial.printf("Playing: %s\n", filename);
    
    if (file) {
        file->close();
        delete file;
    }

    file = new AudioFileSourceSD(filename);
    if (!file) {
        Serial.printf("Error: Failed to open %s!\n", filename);
        return;
    }

    wav->begin(file, out);
}

// Function to handle the Alert Button (Red Alert)
void handleAlertButton() {
    bool buttonState = digitalRead(ALERT_BUTTON);

    if (buttonState == LOW && lastButtonState == HIGH) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            Serial.println("Alert Button Pressed! Triggering Red Alert.");

            // Stop current playback
            if (wav) {
                wav->stop();
            }

            // Start red alert playback
            playingAlert = true;
            alertPlayCount = 0;
            currentState = PLAY_ALERT;
            playFile("/red_alert.wav");

            // Turn on LED blinking and light pushbutton
            digitalWrite(LED_PIN, HIGH);
            digitalWrite(BUTTON_LED_PIN, HIGH);

            lastDebounceTime = millis();
        }
    }
    lastButtonState = buttonState;
}

// Function to blink LED at a fixed rate
void blinkLED() {
    static unsigned long lastBlinkTime = 0;
    static bool ledState = LOW;
    const unsigned long blinkInterval = 500;  // 500ms on/off

    if (millis() - lastBlinkTime >= blinkInterval) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        lastBlinkTime = millis();
    }
}


