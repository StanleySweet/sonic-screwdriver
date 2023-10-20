#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <pins_arduino.h>

#include <cstddef>
#include <string>

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

extern const volatile unsigned long INTERRUPT_DEBOUCE_TIME;
extern const int IR_RECEIVER_BUFFER_LENGTH;
extern const int IR_RECEIVER_TIMEOUT;
extern const unsigned long WAIT_FOR_INPUT_DELAY;
extern const unsigned long BUTTON_RELEASE_TIMEOUT;
extern const unsigned long INACTIVITY_TIMEOUT;
extern const unsigned long PROGRAMMING_TIME;
extern const unsigned long PROGRAMMING_TIMEOUT;
extern const unsigned long BATTERY_PRINT_INTERVAL;
extern const std::string MAGIC;
extern const std::size_t EEPROM_SIZE;

#endif // CONSTANTS_H
