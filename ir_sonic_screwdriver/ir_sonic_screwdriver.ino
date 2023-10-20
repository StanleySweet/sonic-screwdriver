#include "include/ButtonState.h"
#include "include/BuzzerState.h"
#include "include/Constants.h"
#include "include/NeoPixelColor.h"
#include "include/ScrewDriver.h"
#include "include/Pitches.h"
#include "include/Utils.h"

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include <esp_sleep.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>
#include <IRsend.h>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>

int melody[] = {
  Pitches::Note_F7,
  Pitches::Note_G4,
  Pitches::Note_A2
};

int noteDurations[] = {
  NOTE_DURATION_FACTOR / 9,
  NOTE_DURATION_FACTOR / 12,
  NOTE_DURATION_FACTOR / 7
};
char str[80];
ScrewDriver::StatePtr STATE;
unsigned long previousMillis = 0;
unsigned long previousSendTime = 0;
std::string magic = "K-9";

void on_button_change()
{
  unsigned long currentTime = millis();
  unsigned long timeSinceLastInterrupt = currentTime - STATE->InterruptLastTime;
  if (INTERRUPT_DEBOUCE_TIME > timeSinceLastInterrupt)
    return;
  STATE->InterruptLastTime = currentTime;

  unsigned long elapsed = millis() - STATE->Button->LastPressTime;
  read_button();
  STATE->WaitingForInput = true;

  if (STATE->Button->PreviousState == LOW && STATE->Button->CurrentState == HIGH)
    STATE->Button->LastPressTime = currentTime;
  if (STATE->Button->PreviousState == HIGH && STATE->Button->CurrentState == LOW)
  {
    STATE->Button->LastReleaseTime = currentTime;
    ++STATE->Button->PressCount;
  }
}

void read_button()
{
  STATE->Button->PreviousState = STATE->Button->CurrentState;
  STATE->Button->CurrentState = digitalRead(COMMAND_BUTTON);
}

void setup_state()
{
  Serial.write("State init...");
  STATE = std::make_shared<ScrewDriver::State>(PIN_NEOPIXEL);
  pinMode(PIN_NEOPIXEL, OUTPUT);
  Serial.write("done.\n");
}

void setup_serial()
{
  Serial.begin(115200);
  while (!Serial) // Wait for the serial connection to be establised.
    delay(50);
  Serial.write("Serial init... done\n");
}

void setup_builtin_led()
{
  Serial.write("Builtin LED init...");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.write("done.\n");
}

void setup_battery_monitor()
{
  Serial.write("Battery monitor init...");
  pinMode(BATTERY_PIN, INPUT);
  Serial.write("done.\n");
}

void setup_tone_output()
{
  Serial.write("Tone output init...");
  pinMode(TONE_PIN, OUTPUT);
  Serial.write("done.\n");
}

void setup_eeprom()
{
  Serial.write("EEPROM init... ");
  EEPROM.begin(512);

  for (int i = 0; i < magic.size(); ++i) {
    if(EEPROM.readChar(i) != magic[i])
      EEPROM.writeChar(i, magic[i]);
  }
  EEPROM.commit();
  Serial.write("done.\n");
  sprintf(str, "EEPROM size: %d bytes.\n", EEPROM.length());
  Serial.write(str);
  for (size_t mode_index = 0; mode_index < STATE->Signals.size(); ++mode_index)
  {
    decode_results signal;
    size_t address = mode_index * sizeof(decode_results) + magic.size();
    EEPROM.get(address, signal);
    if (signal.decode_type != decode_type_t::UNUSED && signal.decode_type != decode_type_t::UNKNOWN)
    {
      sprintf(str, "Recovered signal for mode '%zu'!\n", mode_index);
      Serial.write(str);
      print_mode_data(signal);
      STATE->Signals[mode_index] = signal;
    }
  }
}

void setup_pixel_light()
{
  STATE->NeoPixelBuiltin->begin();
  STATE->NeoPixelBuiltin->setBrightness(10);
}

float get_battery_level()
{
  return 100.0f * get_battery_raw() / 4095.0f;
}

float get_battery_voltage()
{
  return 1.1f * 3.3f * 2.0f * get_battery_raw() / 4095.0f;
}

int get_battery_raw()
{
  return analogRead(BATTERY_PIN);
}

void print_battery_level()
{
  sprintf(str, "Battery level: %f%\n", get_battery_level());
  Serial.write(str);
  sprintf(str, "Battery voltage: %f%\n", get_battery_voltage());
  Serial.write(str);
  sprintf(str, "Battery pin raw: %d%\n", get_battery_raw());
  Serial.write(str);
}

void setup_ir_receiver()
{
  Serial.write("IR Receiver init... ");
  pinMode(IR_RECEIVE_PIN, INPUT);
  STATE->IRReceiver = std::make_shared<IRrecv>(IR_RECEIVE_PIN, IR_RECEIVER_BUFFER_LENGTH, 50, true);
  Serial.write("done.\n");
}

void setup_ir_sender()
{
  Serial.write("IR Sender init... ");
  pinMode(IR_SEND_PIN, OUTPUT);
  STATE->IRSender = std::make_shared<IRsend>(IR_SEND_PIN);
  STATE->IRSender->begin();
  Serial.write("done.\n");
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup_command_button(){
  Serial.write("Command button init... ");
  pinMode(COMMAND_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(COMMAND_BUTTON), on_button_change, CHANGE);
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(COMMAND_BUTTON), LOW);
  Serial.write("done.\n");
}

void setup_screwdriver_light(){
  Serial.write("Screwdriver light init... ");
  pinMode(SCREWDRIVER_LIGHT_PIN, OUTPUT);
  Serial.write("done.\n");
}


void setup()
{
  setup_serial();
  print_wakeup_reason();
  setup_state();
  setup_builtin_led();
  setup_battery_monitor();
  setup_eeprom();
  setup_pixel_light();
  setup_ir_receiver();
  setup_ir_sender();
  setup_screwdriver_light();
  setup_command_button();
  setup_tone_output();
  print_battery_level();
  Serial.println(D_STR_LIBRARY " : v" _IRREMOTEESP8266_VERSION_STR "\n");
}

void run_idle_mode()
{
  if (STATE->PreviousMode == ScrewDriver::Mode::IDLE)
    return;

  sprintf(str, "Running idle mode!\n");
  Serial.write(str);
  STATE->SetNeoPixelColor(NeoPixel::Colors::Red);
}

void run_sending_mode()
{
  int mode = std::min(STATE->Button->PressCount - 2, static_cast<int>(STATE->Signals.size() - 1));
  decode_results signal = STATE->Signals[mode];
  if (!STATE->IsRunningSubRoutine)
  {
    sprintf(str, "Running sending mode '%d'!\n", mode);
    Serial.write(str);
    print_mode_data(signal);
  }
  STATE->IsRunningSubRoutine = true;
  Serial.write(str);
  switch (mode)
  {
  case 0:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Blue);
    Utils::IntervalBlink(SCREWDRIVER_LIGHT_PIN, 100, &previousMillis);
    break;
  case 1:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Freya::DarkRed);
    Utils::IntervalBlink(SCREWDRIVER_LIGHT_PIN, 200, &previousMillis);
    break;
  case 2:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Freya::DarkGreen);
    Utils::IntervalBlink(SCREWDRIVER_LIGHT_PIN, 400, &previousMillis);
    break;
  case 3:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Freya::DarkBlue);
    Utils::IntervalBlink(SCREWDRIVER_LIGHT_PIN, 600, &previousMillis);
    break;
  }

  unsigned long current_time = millis();

  if (STATE->Buzzer->LastToneTime + noteDurations[0] + noteDurations[1] + noteDurations[2] <= current_time)
  {
    for (int thisNote = 0; thisNote < 3; thisNote++)
    {
      tone(TONE_PIN, melody[thisNote], noteDurations[thisNote]);
    }

    STATE->Buzzer->LastToneTime = current_time;

    Utils::IntervalAction(
      [signal]
      {
        STATE->IRSender->send(signal.decode_type, signal.value, signal.bits, 0U);
      },
      1000,
      &previousSendTime
    );
  }
}

void run_programming_mode()
{
  if (!STATE->IsRunningSubRoutine)
  {
    sprintf(str, "Running programming mode!\n");
    Serial.write(str);
  }

  tone(TONE_PIN, melody[0], 200UL);
  tone(TONE_PIN, melody[2], 200UL);
  delay(400UL);
  STATE->IsRunningSubRoutine = true;
  STATE->IsProgramming = true;
  STATE->SetNeoPixelColor(NeoPixel::Colors::Yellow);
  STATE->ResetButtonPressCount();
  STATE->ResetButtonTime();
}

void leave_programming_mode()
{
  read_button();
  if (STATE->Button->CurrentState == LOW)
    return;

  sprintf(str, "Leaving programming mode!\n");
  Serial.write(str);
  STATE->IsRunningSubRoutine = false;
  STATE->ResetButtonTime();
  STATE->ResetButtonPressCount();
  STATE->SetMode(ScrewDriver::Mode::IDLE);
}

void leave_sending_mode()
{
  read_button();
  if (STATE->Button->CurrentState == LOW)
    return;

  sprintf(str, "Leaving sending mode!\n");
  Serial.write(str);
  STATE->IsRunningSubRoutine = false;
  STATE->SetMode(ScrewDriver::Mode::IDLE);
  digitalWrite(SCREWDRIVER_LIGHT_PIN, LOW);
  STATE->ResetButtonTime();
  STATE->ResetButtonPressCount();
}

void leave_idle_mode()
{
  STATE->PreviousMode = STATE->CurrentMode;
}

void start_programmming_mode()
{
  int mode = std::min(STATE->Button->PressCount - 1, static_cast<int>(STATE->Signals.size() - 1));
  sprintf(str, "Starting programming mode %d!\n", mode);
  Serial.write(str);
  switch (mode)
  {
  case 0:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Purple);
    break;
  case 1:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Aqua);
    break;
  case 2:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Mint);
    break;
  case 3:
    STATE->SetNeoPixelColor(NeoPixel::Colors::Green);
    break;
  }
  read_ir_signal(mode);
  tone(TONE_PIN, melody[0], 200UL);
  tone(TONE_PIN, melody[1], 200UL);
  STATE->IsProgramming = false;
  STATE->WaitingForInput = false;
  print_mode_data(STATE->Signals[mode]);
  STATE->ResetButtonPressCount();
}

void print_mode_data(decode_results signal)
{
  Serial.print(resultToHumanReadableBasic(&signal));
}

void read_ir_signal(int mode)
{
  STATE->IRReceiver->enableIRIn();

  bool signalReceived = false;
  while (!signalReceived)
  {
    decode_results result;
    if (STATE->IRReceiver->decode(&result, nullptr, 0U, 0U))
    {
      if (result.overflow)
      {
        Serial.printf(D_WARN_BUFFERFULL "\n", IR_RECEIVER_BUFFER_LENGTH);
        yield();
        continue;
      }

      if (result.bits > 10)
      {
        if (result.value != STATE->Signals[mode].value)
        {
          STATE->Signals[mode] = result;
          signalReceived = true;
          EEPROM.put(mode * sizeof(decode_results) + magic.size(), result);
          EEPROM.commit();
        }
        else
        {
          Serial.printf("Same old, same old.\n");
          signalReceived = true;
        }
      }
    }
    yield();
  }
  STATE->IRReceiver->disableIRIn();
}

void run_inactive_mode()
{
  sprintf(str, "Device is now inactive!\n");
  Serial.write(str);
  esp_deep_sleep_start();
}

void loop()
{
  unsigned long current_time = millis();
  read_button();
  if (STATE->CurrentMode == ScrewDriver::Mode::IDLE && STATE->Button->CurrentState == HIGH)
  {
    if (current_time - STATE->LastBatteryPrintTime > BATTERY_PRINT_INTERVAL)
    {
      print_battery_level();
      STATE->LastBatteryPrintTime = current_time;
    }

    if (current_time - STATE->Button->LastReleaseTime > BUTTON_RELEASE_TIMEOUT)
    {
      STATE->ResetButtonPressCount();
    }

    if (current_time - STATE->Button->LastReleaseTime > INACTIVITY_TIMEOUT)
    {
      run_inactive_mode();
    }
  }

  if (STATE->WaitingForInput)
  {
    unsigned long elapsed = current_time - STATE->Button->LastPressTime;
    if (STATE->CurrentMode != ScrewDriver::Mode::PROGRAMMING)
    {
      if(STATE->Button->CurrentState == HIGH)
      {
        switch(STATE->Button->PressCount)
        {
          case 1:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Blue);
            break;
          case 2:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Freya::DarkRed);
            break;
          case 3:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Freya::DarkGreen);
            break;
          case 4:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Freya::DarkBlue);
            break;
        }
      }

      if (elapsed >= PROGRAMMING_TIME && STATE->Button->CurrentState == LOW && STATE->Button->PressCount == 1)
      {
        STATE->SetMode(ScrewDriver::Mode::PROGRAMMING);
        run_programming_mode();
      }
      else if (elapsed >= WAIT_FOR_INPUT_DELAY && STATE->Button->PressCount > 1 && STATE->Button->CurrentState == LOW)
      {
        STATE->SetMode(ScrewDriver::Mode::SENDING);
        STATE->WaitingForInput = false;
      }
    }
    else
    {
      if(STATE->Button->CurrentState == HIGH)
      {
          switch (STATE->Button->PressCount)
          {
          case 1:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Purple);
            break;
          case 2:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Aqua);
            break;
          case 3:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Mint);
            break;
          case 4:
            STATE->SetNeoPixelColor(NeoPixel::Colors::Green);
            break;
          }
      }


      if (elapsed >= WAIT_FOR_INPUT_DELAY && STATE->Button->PressCount > 1 && STATE->Button->CurrentState == HIGH)
      {
        start_programmming_mode();
      }
    }
  }
  else
  {
    STATE->ResetButtonTime();
  }

  switch (STATE->CurrentMode)
  {
  case ScrewDriver::Mode::PROGRAMMING:
    if (!STATE->IsProgramming)
      leave_programming_mode();
    break;
  case ScrewDriver::Mode::SENDING:
    run_sending_mode();
    leave_sending_mode();
    break;
  case ScrewDriver::Mode::IDLE:
    run_idle_mode();
    leave_idle_mode();
    break;
  }
}
