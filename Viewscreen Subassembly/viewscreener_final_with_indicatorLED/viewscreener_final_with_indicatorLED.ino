// Master Animated GIF on a 320x240 SPI LCD Screen
// Youtube Tutorial: https://youtu.be/omUWkUqFYrQ
// Tested with Espressif ESP32 Arduino Core v3.0.2, 3.0.3
// Using ESP32-S3 with 8MB of PSRAM
// See also the Tutorial on how to create ESP32 custom partitions:

#include "esp_flash.h"
#include "esp_partition.h"

#include <bb_spi_lcd.h>  // Install this library with the Arduino IDE Library Manager
                         // Tested on version 2.5.4, 2.6.0, 2.9.1, CAUTION WITH 2.9.3
#include <AnimatedGIF.h> // Install this library with the Arduino IDE Library Manager
                         // Tested on version 2.1.1 CAUTION with 2.2.0

// GIF files
#include "gif_files/alien_head.h"
#include "gif_files/aiship.h"
#include "gif_files/apollo.h"
#include "gif_files/rigel.h"
//#include "gif_files/cruising.h"
#include "gif_files/doomsday.h"
#include "gif_files/enceladus.h"
#include "gif_files/enterprise_status.h"
#include "gif_files/hand.h"
//#include "gif_files/high_altitude.h"
#include "gif_files/k7.h"
#include "gif_files/kirk.h"
#include "gif_files/klingon_ship.h"
#include "gif_files/lincoln.h"
#include "gif_files/maudbox.h"
#include "gif_files/mining.h"
#include "gif_files/neutralzone.h"
#include "gif_files/opencredits.h"
#include "gif_files/quantum.h"
#include "gif_files/romulan_green.h"
#include "gif_files/warp_oscillate.h"
//#include "gif_files/brain.h"
#include "gif_files/corbomite_cube.h"
#include "gif_files/amorphous_cloud.h"
#include "gif_files/tongue_ball.h"
#include "gif_files/wormy_stars.h"
#include "gif_files/saucer.h"
#include "gif_files/galaxy.h"
#include "gif_files/corbomite_ship.h"
#include "gif_files/star_field.h"
#include "gif_files/tunnel.h"
#include "gif_files/alien_fleet_command.h"



// Push Button
#define PUSH_BUTTION_PIN 6
unsigned long lastDebounceTime;
int lastState = -1;
int state = -1;

#define INDICATOR_LED_PIN 10 //indicator LED for when button is pushed

// Main object for the display driver
BB_SPI_LCD tft;
// GIFs to display
#define GIF_COUNT 29 // Number of GIFs to cycle through, if you have enough space on flash memory
const uint8_t *gifData[GIF_COUNT] = {star_field, alien_head, aiship, apollo, rigel, doomsday, enceladus, enterprise_status, hand,
  k7, kirk, klingon_ship, lincoln, maudbox, mining, neutralzone, opencredits, quantum, romulan_green, 
  warp_oscillate, corbomite_cube, amorphous_cloud, tongue_ball, wormy_stars, saucer, galaxy, corbomite_ship, 
  tunnel, alien_fleet_command};                // Add more GIFs here if you have enough space on flash memory
const size_t gifSizes[GIF_COUNT] = {sizeof(star_field), sizeof(alien_head), sizeof(aiship), sizeof(apollo), sizeof(rigel), sizeof(doomsday), sizeof(enceladus), sizeof(enterprise_status), sizeof(hand),  
  sizeof(k7), sizeof(kirk), sizeof(klingon_ship), sizeof(lincoln), sizeof(maudbox), sizeof(mining), sizeof(neutralzone), sizeof(opencredits), sizeof(quantum), sizeof(romulan_green), 
  sizeof(warp_oscillate), sizeof(corbomite_cube), sizeof(amorphous_cloud), sizeof(tongue_ball), sizeof(wormy_stars), sizeof(saucer), sizeof(galaxy), sizeof(corbomite_ship), 
  sizeof(tunnel), sizeof(alien_fleet_command)}; // Add corresponding sizes here
AnimatedGIF *gifTopPlay[GIF_COUNT];
int currentGifPlayed = 0;

void setup()
{
  pinMode(PUSH_BUTTION_PIN, INPUT_PULLUP);
  lastDebounceTime = millis();

  digitalWrite(INDICATOR_LED_PIN, LOW); // Start with indicator LED off
  pinMode(INDICATOR_LED_PIN, OUTPUT);

  Serial.begin(115200);
  tft.begin(LCD_ILI9341, FLAGS_NONE, 40000000, 8, 18, 17, -1, -1, 9, 3); //
  tft.setRotation(LCD_ORIENTATION_90);                                   // Make sure you have the right orientation based on your GIF
                                                                         // or the GIF will show incorrectly, even garbage output
                                                                         // Values : LCD_ORIENTATION_0, LCD_ORIENTATION_90, LCD_ORIENTATION_180 or LCD_ORIENTATION_270
  tft.fillScreen(TFT_BLACK);

  for (int i = 0; i < GIF_COUNT; i++)
  {
    gifTopPlay[i] = openGif((uint8_t *)gifData[i], gifSizes[i]);
    if (gifTopPlay[i] == NULL)
    {
      Serial.println("Cannot open GIF");
      while (true)
      {
        // No need to continue
      }
    }
  }
}

void loop()
{
  if (ButtonPressed())
  {
    digitalWrite(INDICATOR_LED_PIN, HIGH); // light the indicator
    Serial.println("Button Pressed, showing the next GIF");
    tft.fillScreen(TFT_BLACK);
    currentGifPlayed = (currentGifPlayed + 1) % GIF_COUNT; // Cycle through the GIFs
  }

  // turn off LED when button is released
  if (!digitalRead(PUSH_BUTTION_PIN)) // Assuming LOW means not pressed
{
  digitalWrite(INDICATOR_LED_PIN, LOW);
}
  gifTopPlay[currentGifPlayed]->playFrame(false, NULL);
}

// Open Gif and allocate memory
AnimatedGIF *openGif(uint8_t *gifdata, size_t gifsize)
{
  AnimatedGIF *gif;
  gif = (AnimatedGIF *)malloc(sizeof(AnimatedGIF));
  if (gif == NULL)
  {
    Serial.println("Not RAM Enough memory for GIF structure");
    return NULL;
  }

  gif->begin(GIF_PALETTE_RGB565_BE); // Set the cooked output type we want (compatible with SPI LCDs)

  if (gif->open(gifdata, gifsize, GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif->getCanvasWidth(), gif->getCanvasHeight());
    Serial.printf("GIF memory size is %ld (%2.2f MB)", gifsize, (float)gifsize / (1024 * 1024));
    gif->setDrawType(GIF_DRAW_COOKED); // We want the Animated GIF library to generate ready-made pixels
    if (gif->allocFrameBuf(GIFAlloc) != GIF_SUCCESS)
    {
      Serial.println("Not Enough RAM memory for frame buffer");
      return NULL;
    }
    return gif;
  }
  else
  {
    printGifErrorMessage(gif->getLastError());
    return NULL;
  }
}

bool ButtonPressed()
{
  int currentState = digitalRead(PUSH_BUTTION_PIN);
  if (currentState != lastState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 1)
  {
    if (currentState != state)
    {
      state = currentState;
      lastState = currentState;
      return state == LOW; // Returns true if the button is pressed
    }
  }

  lastState = currentState;
  return false; // No change in state
}

//
// The memory management functions are needed to keep operating system
// dependencies out of the core library code
//
// memory allocation callback function
void *GIFAlloc(uint32_t u32Size)
{
  return malloc(u32Size);
} /* GIFAlloc() */
// memory free callback function
void GIFFree(void *p)
{
  free(p);
}

// Draw callback from the AnimatedGIF decoder
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0)
  { // set the memory window (once per frame) when the first line is rendered
    tft.setAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  // For all other lines, just push the pixels to the display. We requested 'COOKED'big-endian RGB565 and
  tft.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
}

// Get human-readable error related to GIF
void printGifErrorMessage(int errorCode)
{
  switch (errorCode)
  {
  case GIF_DECODE_ERROR:
    Serial.println("GIF Decoding Error");
    break;
  case GIF_TOO_WIDE:
    Serial.println("GIF Too Wide");
    break;
  case GIF_INVALID_PARAMETER:
    Serial.println("Invalid Parameter for gif open");
    break;
  case GIF_UNSUPPORTED_FEATURE:
    Serial.println("Unsupported feature in GIF");
    break;
  case GIF_FILE_NOT_OPEN:
    Serial.println("GIF File not open");
    break;
  case GIF_EARLY_EOF:
    Serial.println("GIF early end of file");
    break;
  case GIF_EMPTY_FRAME:
    Serial.println("GIF with empty frame");
    break;
  case GIF_BAD_FILE:
    Serial.println("GIF bad file");
    break;
  case GIF_ERROR_MEMORY:
    Serial.println("GIF memory Error");
    break;
  default:
    Serial.println("Unknown Error");
    break;
  }
}