#include "../include/ScrewDriver.h"
#include "../include/NeoPixelColor.h"

#include <Adafruit_NeoPixel.h>

#include <algorithm>

ScrewDriver::State::State(int16_t neoPixelPin)
{
    this->WaitingForInput = false;
    this->IsRunningSubRoutine = false;
    this->IsProgramming = false;
    this->InterruptLastTime = 0;
    this->LastBatteryPrintTime = 0;
    this->Button = std::make_shared<ButtonState>();
    this->Button->CurrentState = HIGH;
    this->Button->PreviousState = LOW;
    this->Button->PressCount = 0;
    this->Button->LastPressTime = 0;
    this->InterruptLastTime = millis();
    this->NeoPixelBuiltin = std::make_shared<Adafruit_NeoPixel>(1, neoPixelPin);
    this->Buzzer = std::make_shared<BuzzerState>();
    this->PreviousMode = ScrewDriver::Mode::NONE;
    this->CurrentMode = ScrewDriver::Mode::IDLE;
    std::fill(this->Signals.begin(), this->Signals.end(), decode_results());
}

void ScrewDriver::State::SetMode(ScrewDriver::Mode newMode)
{
    if (this->CurrentMode == newMode || this->IsRunningSubRoutine)
        return;

    this->PreviousMode = this->CurrentMode;
    this->CurrentMode = newMode;
}

void ScrewDriver::State::ResetButtonTime()
{
    this->Button->LastPressTime = millis();
    this->Button->LastReleaseTime = millis();
}

void ScrewDriver::State::ResetButtonPressCount()
{
    this->Button->PressCount = 0;
}

void ScrewDriver::State::SetNeoPixelColor(NeoPixel::Color color)
{
    this->NeoPixelBuiltin->setPixelColor(0, color.R, color.G, color.B);
    this->NeoPixelBuiltin->show();
}

NeoPixel::Color ScrewDriver::State::GetProgrammingModeColor(uint8_t mode)
{
    switch (mode)
    {
    case 0:
        return NeoPixel::Colors::Purple;
    case 1:
        return NeoPixel::Colors::Aqua;
    case 2:
        return NeoPixel::Colors::Mint;
    case 3:
        return NeoPixel::Colors::Green;
    default:
        return NeoPixel::Colors::White;
    }
}

NeoPixel::Color ScrewDriver::State::GetSendingModeColor(uint8_t mode)
{
    switch (mode)
    {
    case 0:
        return NeoPixel::Colors::Blue;
    case 1:
        return NeoPixel::Colors::Freya::DarkRed;
    case 2:
        return NeoPixel::Colors::Freya::DarkGreen;
    case 3:
        return NeoPixel::Colors::Freya::DarkBlue;
    default:
        return NeoPixel::Colors::White;
    }
}
