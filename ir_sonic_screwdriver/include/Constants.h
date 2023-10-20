#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <pins_arduino.h>

#define USE_BUILTIN_BUTTON 0

#ifndef PIN_NEOPIXEL
const uint8_t PIN_NEOPIXEL = 0;
#endif

#ifndef LED_BUILTIN
const uint8_t LED_BUILTIN = 24;
#endif

#ifndef SCREWDRIVER_LIGHT_PIN
const uint8_t SCREWDRIVER_LIGHT_PIN = 32;
#endif

#ifndef IR_RECEIVE_PIN
const uint8_t IR_RECEIVE_PIN = 14;
#endif

#ifndef BATTERY_PIN
const uint8_t BATTERY_PIN = A13;
#endif

#ifndef IR_SEND_PIN
const uint8_t IR_SEND_PIN = A5;
#endif

#ifndef TONE_PIN
const uint8_t TONE_PIN = 15;
#endif

#ifndef BUTTON || defined(USE_BUILTIN_BUTTON) && USE_BUILTIN_BUTTON
const uint8_t BUTTON = 38;
#else
#ifndef COMMAND_BUTTON
const uint8_t COMMAND_BUTTON = 27;
#endif
#endif

constexpr volatile unsigned long INTERRUPT_DEBOUCE_TIME = 50;
constexpr volatile unsigned long WAIT_FOR_INPUT_DELAY = 750;
constexpr volatile unsigned long BUTTON_RELEASE_TIMEOUT = 2000;
constexpr volatile unsigned long INACTIVITY_TIMEOUT = 6000;
constexpr volatile unsigned long PROGRAMMING_TIME = 5000;
constexpr volatile int IR_RECEIVER_BUFFER_LENGTH = 1024;
constexpr volatile int IR_RECEIVER_TIMEOUT = 50;
const int NOTE_DURATION_FACTOR = 275;
const unsigned long BATTERY_PRINT_INTERVAL = 10 * 1000;

#endif // CONSTANTS_H
