#include "../include/ScrewDriver.h"
#include "../include/NeoPixelColor.h"

#include <Adafruit_NeoPixel.h>

#include <algorithm>

ScrewDriver::State::State(int16_t neoPixelPin)
{
    this->WaitingForInput = false;
    this->IsRunningSubRoutine = false;
    this->IsProgramming = false;
    this->InterruptLastTime = millis();
    this->LastBatteryPrintTime = millis();
    this->LastProgrammingStartTime = millis();
    this->InterruptLastTime = millis();
    this->PreviousMode = ScrewDriver::Mode::NONE;
    this->CurrentMode = ScrewDriver::Mode::IDLE;
    this->Button = std::make_shared<ButtonState>();
    this->NeoPixelBuiltin = std::make_shared<Adafruit_NeoPixel>(1, neoPixelPin);
    this->Buzzer = std::make_shared<BuzzerState>();
    std::fill(this->Signals.begin(), this->Signals.end(), decode_results());
}

void ScrewDriver::State::SetMode(ScrewDriver::Mode newMode)
{
    if (this->CurrentMode == newMode || this->IsRunningSubRoutine)
        return;

    this->PreviousMode = this->CurrentMode;
    this->CurrentMode = newMode;
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
    case 4:
        return NeoPixel::Colors::Blue;
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
    case 4:
        return NeoPixel::Colors::Freya::LightRed;
    default:
        return NeoPixel::Colors::White;
    }
}
