# IR Remote Sonic Screwdriver

## Introduction

With the recent hype about Doctor Who's 60th birthday, I wanted to make a little screwdriver of my own. The idea was to have something we can use after the promotional shot for the PR department to keep opening and closing the shutters.

## Roadmap

### Minimum

- [x] Lighting the screwdriver led
- [x] Add sounds to the screwdriver
- [x] Use the NeoPixel led to convey information about modes
- [x] Allow registering signals using an IR receiver
- [x] Allow sending signals using an IR led
- [x] Allow to monitor the battery level through serial

## Bonus

- [x] Save the signals between restarts / disconnections
- [x] Add a low power mode

### Extra Bonus

- [x] Allow to monitor the battery level through the neopixel led
- [ ] Allow remote connexions using the wifi module
- [ ] Allow the screwdriver led to change colors depending on a setting




## Parts

- https://www.amazon.fr/dp/B0BRRVTLV5?psc=1&ref=ppx_yo2ov_dt_b_product_details
- https://www.amazon.fr/dp/B0C746ZVNY?psc=1&ref=ppx_yo2ov_dt_b_product_details
- https://www.amazon.fr/dp/B07449V33P?psc=1&ref=ppx_yo2ov_dt_b_product_details
- https://www.amazon.fr/dp/B0BHJBC2DN?psc=1&ref=ppx_yo2ov_dt_b_product_details
- https://www.amazon.fr/dp/B08DX9Q49J?psc=1&ref=ppx_yo2ov_dt_b_product_details
- https://www.amazon.fr/dp/B0967TG6XR?psc=1&ref=ppx_yo2ov_dt_b_product_details
- https://www.gotronic.fr/art-2-accus-li-ion-14500-5645.htm
- https://www.gotronic.fr/art-carte-feather-esp32-v2-ada5400-35213.htm


## Useful Links:


Original made by the BBC themselves:

- https://www.youtube.com/watch?v=PZcUVGfhJNM

Possible cards

- https://www.amazon.fr/Arduino-Sound-Music-Digital-abx00012/dp/B077NBB23L/ref=sr_1_1?__mk_fr_FR=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=BO0ITVHMHR0I&keywords=mkr+zero&qid=1694190521&sprefix=mkr+zero%2Caps%2C99&sr=8-1
- https://store.arduino.cc/products/arduino-mkr-zero-i2s-bus-sd-for-sound-music-digital-audio-data

Builtin neopixel led

- https://learn.adafruit.com/adafruit-esp32-feather-v2/neopixel-led

Other examples

- https://www.instructables.com/Working-Sonic-Screwdriver-Version-30/
- https://www.instructables.com/Metal-Doctor-Who-Sonic-Screwdriver-with-Arduino/
- https://www.instructables.com/Sonic-Screwdriver/

LED Resistance Calculator

- https://www.hobby-hour.com/electronics/ledcalc.php

EEPROM:
- https://docs.arduino.cc/learn/built-in-libraries/eeprom
- https://arduino.stackexchange.com/questions/25945/how-to-read-and-write-eeprom-in-esp8266
- https://docs.arduino.cc/learn/programming/eeprom-guide
- https://microdigisoft.com/esp32-flash-memory-save-permanent-data-write-and-read/
- https://maker.pro/arduino/tutorial/how-to-permanently-store-data-on-your-arduino
- https://www.instructables.com/Working-Sonic-Screwdriver/
