#include "../include/ButtonState.h"
#include "../include/Constants.h"

#include <Arduino.h>

void ButtonState::StoreValue()
{
    this->PreviousState = this->CurrentState;
    this->CurrentState = digitalRead(COMMAND_BUTTON);
}

ButtonState::ButtonState()
{
    this->Reset();
}

void ButtonState::Reset()
{
    this->CurrentState = HIGH;
    this->PreviousState = LOW;
    this->ResetTime();
    this->ResetPressCount();
}

void ButtonState::ResetTime()
{
    this->LastPressTime = millis();
    this->LastReleaseTime = millis();
}

void ButtonState::ResetPressCount()
{
    this->PressCount = 0;
}
