#include "../include/Constants.h"

const volatile unsigned long INTERRUPT_DEBOUCE_TIME = 50;
const int IR_RECEIVER_BUFFER_LENGTH = 512;
const int IR_RECEIVER_TIMEOUT = 50;
const unsigned long WAIT_FOR_INPUT_DELAY = 750;
const unsigned long BUTTON_RELEASE_TIMEOUT = 2000;
const unsigned long PROGRAMMING_TIME = 5000;
const unsigned long INACTIVITY_TIMEOUT = 10 * 1000;
const unsigned long BATTERY_PRINT_INTERVAL = 10 * 1000;
const unsigned long PROGRAMMING_TIMEOUT = 20 * 1000;
const std::string MAGIC = "K-9";
const std::size_t EEPROM_SIZE = 512;
