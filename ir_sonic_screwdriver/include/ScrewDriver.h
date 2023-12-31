#ifndef SCREWDRIVER_H
#define SCREWDRIVER_H

#include "ButtonState.h"
#include "BuzzerState.h"

#include <IRrecv.h>

#include <array>
#include <cstdint>
#include <memory>

struct decode_results;
class Adafruit_NeoPixel;
class IRsend;
namespace NeoPixel
{
  class Color;
}

namespace ScrewDriver
{
  enum Mode
  {
    NONE = 0,
    IDLE = 1,
    SENDING = 2,
    PROGRAMMING = 3
  };

  using ButtonStatePtr = std::shared_ptr<ButtonState>;
  using IRsendPtr = std::shared_ptr<IRsend>;
  using IRrecvPtr = std::shared_ptr<IRrecv>;
  using BuzzerStatePtr = std::shared_ptr<BuzzerState>;
  using AdafruitNeoPixelPtr = std::shared_ptr<Adafruit_NeoPixel>;

  class State
  {
  public:
    bool IsProgramming;
    bool IsRunningSubRoutine;
    volatile bool WaitingForInput;
    volatile unsigned long InterruptLastTime;
    unsigned long LastBatteryPrintTime;
    unsigned long LastProgrammingStartTime;
    unsigned long PreviousBlinkTime;
    unsigned long PreviousSendTime;
    Mode CurrentMode;
    Mode PreviousMode;
    IRrecvPtr IRReceiver;
    IRsendPtr IRSender;
    ButtonStatePtr Button;
    AdafruitNeoPixelPtr NeoPixelBuiltin;
    BuzzerStatePtr Buzzer;
    std::array<decode_results, 5U> Signals;
    State(int16_t neoPixelPin);
    void SetMode(Mode newMode);
    void SetNeoPixelColor(NeoPixel::Color color);
    NeoPixel::Color GetProgrammingModeColor(uint8_t mode);
    NeoPixel::Color GetSendingModeColor(uint8_t mode);
  };

  using StatePtr = std::shared_ptr<ScrewDriver::State>;
}
#endif // SCREWDRIVER_H
