#ifndef UTILS_H
#define UTILS_H

#include "../include/Utils.h"

#include <Arduino.h>

void Utils::IntervalAction(std::function<void()> func, unsigned long interval, unsigned long *previousExecutionTime)
{
    unsigned long current_time = millis(); // Get the current timestamp
    if (current_time - *previousExecutionTime >= interval)
    {
        *previousExecutionTime = current_time;
        func();
    }
}

#endif // UTILS_H
