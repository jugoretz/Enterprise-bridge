# Enterprise-bridge
TOS USS Enterprise Bridge Model Code

Based on the AMT Model Kit (with the two paragrafix photoetch and screen transparencies), this version includes sounds, viewscreen with animated GIFs, lots of blinking lights, an animated moire screen, and sounds.

Be sure to watch the video:
https://www.youtube.com/watch?v=i5-EIJWvTqg&t=13s

## Components:

### Viewscreen Subassembly
- ESP32-S3 microcontroller
- 2.4" ILI9341 TFT display
- Pushbutton
- LED

### Chaser/Panels Subassembly 
- ESP32-S3 microcontroller
- 15 LED chaser 
- additional LEDs

### Audio + Red Alert Subassembly
- ESP32-C3 microcontroller
- MAX98357 I2S amplifier 
- Micro SD card reader 
- Pushbutton
- LEDs 
- Mosfetti board

### Blinky Subassembly
- ESP32-C3 microcontroller
- Mosfetti board
- tons of LEDs

### Moiré Subassembly
- ESP32-C3 microcontroller
- SSD1306 OLED display

### Breather Subassembly
- ESP32-C3 microcontroller
- Single LED (blue)

### Constant Subassembly
- 50+ LEDs (including nOOds)

### Power & Enclosure System
- 5V 10A wall power supply
- Rocker switch between power supply and input terminal 
- 12-terminal power distribution bar 
- Main power LED 

(all LEDs have 220ohm resistors)
(and **lots** of wire and **LOTS** of fiber optics)
