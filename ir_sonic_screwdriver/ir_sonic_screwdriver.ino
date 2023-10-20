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
#include <array>
#include <cstdint>
#include <functional>
#include <memory>

char str[80];
ScrewDriver::StatePtr STATE;

void on_button_change()
{
  unsigned long currentTime = millis();
  unsigned long timeSinceLastInterrupt = currentTime - STATE->InterruptLastTime;
  if (INTERRUPT_DEBOUCE_TIME > timeSinceLastInterrupt)
    return;
  STATE->InterruptLastTime = currentTime;

  unsigned long elapsed = currentTime - STATE->Button->LastPressTime;
  STATE->Button->StoreValue();
  STATE->WaitingForInput = true;

  if (STATE->Button->PreviousState == LOW && STATE->Button->CurrentState == HIGH)
    STATE->Button->LastPressTime = currentTime;
  if (STATE->Button->PreviousState == HIGH && STATE->Button->CurrentState == LOW)
  {
    STATE->Button->LastReleaseTime = currentTime;
    ++STATE->Button->PressCount;
  }
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
  EEPROM.begin(EEPROM_SIZE);

  for (int i = 0; i < MAGIC.size(); ++i)
  {
    if (EEPROM.readChar(i) != MAGIC[i])
      EEPROM.writeChar(i, MAGIC[i]);
  }
  EEPROM.commit();
  Serial.write("done.\n");
  sprintf(str, "EEPROM size: %d bytes.\n", EEPROM.length());
  Serial.write(str);
  for (size_t mode_index = 0; mode_index < STATE->Signals.size(); ++mode_index)
  {
    decode_results signal;
    size_t address = mode_index * sizeof(decode_results) + MAGIC.size();
    EEPROM.get(address, signal);
    if (signal.decode_type != decode_type_t::UNUSED && signal.decode_type != decode_type_t::UNKNOWN)
    {
      sprintf(str, "Recovered signal for mode '%zu'.\n", mode_index);
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
  STATE->IRReceiver = std::make_shared<IRrecv>(IR_RECEIVE_PIN, IR_RECEIVER_BUFFER_LENGTH, IR_RECEIVER_TIMEOUT, true);
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

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void setup_command_button()
{
  Serial.write("Command button init... ");
  pinMode(COMMAND_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(COMMAND_BUTTON), on_button_change, CHANGE);
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(COMMAND_BUTTON), LOW);
  Serial.write("done.\n");
}

void setup_screwdriver_light()
{
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
  Serial.print(D_STR_LIBRARY " : v" _IRREMOTEESP8266_VERSION_STR "\n");
}

void run_idle_mode()
{
  if (STATE->PreviousMode == ScrewDriver::Mode::IDLE)
    return;

  sprintf(str, "Running idle mode.\n");
  Serial.write(str);
  STATE->SetNeoPixelColor(NeoPixel::Colors::Red);
}

void run_sending_mode()
{
  int mode = std::min(STATE->Button->PressCount - 2, static_cast<int>(STATE->Signals.size() - 1));
  decode_results signal = STATE->Signals[mode];
  if (!STATE->IsRunningSubRoutine)
  {
    sprintf(str, "Running sending mode '%d'.\n", mode);
    Serial.write(str);
    print_mode_data(signal);
    STATE->IsRunningSubRoutine = true;
  }

  uint8_t repeat = static_cast<uint8_t>(std::ceil(1000.0 / STATE->Buzzer->GetToneDuration()));
  unsigned long sendInterval = STATE->Buzzer->GetToneDuration() * (repeat - 1);
  Utils::IntervalAction([]
                        { digitalWrite(SCREWDRIVER_LIGHT_PIN, digitalRead(SCREWDRIVER_LIGHT_PIN) == HIGH ? LOW : HIGH); },
                        100LU * (mode + 1LU),
                        &STATE->PreviousBlinkTime);
  Utils::IntervalAction([repeat]
                        { STATE->Buzzer->Play(repeat); },
                        STATE->Buzzer->GetToneDuration(),
                        &STATE->Buzzer->LastToneTime);
  Utils::IntervalAction([signal]
                        { STATE->IRSender->send(signal.decode_type, signal.value, signal.bits, 1U); },
                        sendInterval,
                        &STATE->PreviousSendTime);
}

void run_programming_mode()
{
  if (!STATE->IsRunningSubRoutine)
  {
    sprintf(str, "Running programming mode.\n");
    Serial.write(str);
    STATE->LastProgrammingStartTime = millis();
  }
  tone(TONE_PIN, STATE->Buzzer->ToneData[0].Frequency, 200UL);
  tone(TONE_PIN, STATE->Buzzer->ToneData[2].Frequency, 200UL);
  STATE->IsRunningSubRoutine = true;
  STATE->IsProgramming = true;
  STATE->SetNeoPixelColor(NeoPixel::Colors::Yellow);
  STATE->Button->Reset();
}

void leave_programming_mode()
{
  sprintf(str, "Leaving programming mode.\n");
  Serial.write(str);
  STATE->IsRunningSubRoutine = false;
  STATE->Button->Reset();
  STATE->SetMode(ScrewDriver::Mode::IDLE);
}

void leave_sending_mode()
{
  sprintf(str, "Leaving sending mode.\n");
  Serial.write(str);
  digitalWrite(SCREWDRIVER_LIGHT_PIN, LOW);
  STATE->IsRunningSubRoutine = false;
  STATE->Button->Reset();
  STATE->SetMode(ScrewDriver::Mode::IDLE);
}

void leave_idle_mode()
{
  STATE->PreviousMode = STATE->CurrentMode;
}

void start_receiving_mode(int mode)
{
  sprintf(str, "Starting receiving mode '%d'.\n", mode);
  Serial.write(str);
  tone(TONE_PIN, Pitches::Note_G6, 200UL);
  tone(TONE_PIN, Pitches::Note_A6, 200UL);
  read_ir_signal(mode);
  print_mode_data(STATE->Signals[mode]);
  leave_receiving_mode();
}

void leave_receiving_mode()
{
  sprintf(str, "Leaving receiving mode.\n");
  Serial.write(str);
  tone(TONE_PIN, Pitches::Note_G7, 200UL);
  tone(TONE_PIN, Pitches::Note_A7, 200UL);
  STATE->WaitingForInput = false;
  STATE->IsProgramming = false;
  STATE->Button->ResetPressCount();
}

void print_mode_data(decode_results signal)
{
  Serial.print(resultToHumanReadableBasic(&signal));
}

void read_ir_signal(int mode)
{
  STATE->IRReceiver->enableIRIn();

  bool signal_received = false;
  STATE->LastProgrammingStartTime = millis();
  while (!signal_received)
  {
    if (STATE->LastProgrammingStartTime + PROGRAMMING_TIMEOUT < millis())
    {
      break;
    }

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
          signal_received = true;
          EEPROM.put(mode * sizeof(decode_results) + MAGIC.size(), result);
          EEPROM.commit();
        }
        else
        {
          Serial.printf("Same old, same old.\n");
          signal_received = true;
        }
      }
    }
    yield();
  }

  if (signal_received)
    sprintf(str, "Finished receiving mode '%d'.\n", mode);
  else
    sprintf(str, "Cancelled receiving: Timeout exceeded .\n");

  Serial.write(str);

  STATE->IRReceiver->disableIRIn();
}

void run_inactive_mode()
{
  sprintf(str, "Device is now inactive.\n");
  Serial.write(str);
  esp_deep_sleep_start();
}

void loop()
{
  unsigned long current_time = millis();
  unsigned long elapsed_since_last_press = current_time - STATE->Button->LastPressTime;
  unsigned long elapsed_since_last_release = current_time - STATE->Button->LastReleaseTime;
  unsigned long elapsed_since_last_print = current_time - STATE->LastBatteryPrintTime;

  STATE->Button->StoreValue();
  if (STATE->CurrentMode == ScrewDriver::Mode::IDLE && STATE->Button->CurrentState == HIGH)
  {
    Utils::IntervalAction([]
                          { print_battery_level(); },
                          BATTERY_PRINT_INTERVAL, &STATE->LastBatteryPrintTime);

    if (elapsed_since_last_release > BUTTON_RELEASE_TIMEOUT)
    {
      STATE->Button->ResetPressCount();
    }

    if (elapsed_since_last_release > INACTIVITY_TIMEOUT)
    {
      run_inactive_mode();
    }
  }

  if (STATE->WaitingForInput)
  {
    if (STATE->CurrentMode == ScrewDriver::Mode::IDLE)
    {
      if (STATE->Button->CurrentState == HIGH && STATE->Button->PressCount > 0)
      {
        STATE->SetNeoPixelColor(STATE->GetSendingModeColor(STATE->Button->PressCount - 1));
      }

      if (STATE->Button->CurrentState == LOW)
      {
        if (elapsed_since_last_press >= PROGRAMMING_TIME && STATE->Button->PressCount == 1)
        {
          STATE->SetMode(ScrewDriver::Mode::PROGRAMMING);
          run_programming_mode();
        }
        else if (elapsed_since_last_press >= WAIT_FOR_INPUT_DELAY && STATE->Button->PressCount > 1)
        {
          STATE->SetMode(ScrewDriver::Mode::SENDING);
          STATE->WaitingForInput = false;
        }
      }
    }
    else
    {
      if (STATE->Button->CurrentState == HIGH && STATE->CurrentMode == ScrewDriver::Mode::PROGRAMMING)
      {
        int mode = std::min(STATE->Button->PressCount - 2, static_cast<int>(STATE->Signals.size() - 1));
        if (STATE->Button->PressCount > 1)
        {
          if(elapsed_since_last_press >= WAIT_FOR_INPUT_DELAY)
          {
            start_receiving_mode(mode);
          }
          else {
            STATE->SetNeoPixelColor(STATE->GetProgrammingModeColor(mode));
          }
        }
      }
    }
  }
  else
  {
    STATE->Button->ResetTime();
  }

  switch (STATE->CurrentMode)
  {
  case ScrewDriver::Mode::PROGRAMMING:
    if (STATE->Button->CurrentState == HIGH)
    {
      if (!STATE->IsProgramming)
        leave_programming_mode();
      else if (STATE->LastProgrammingStartTime + PROGRAMMING_TIMEOUT < current_time)
      {
        sprintf(str, "Cancelled programming: Timeout exceeded .\n");
        Serial.write(str);
        leave_receiving_mode();
        leave_programming_mode();
      }
    }
    break;
  case ScrewDriver::Mode::SENDING:
    run_sending_mode();
    if (STATE->Button->CurrentState == HIGH)
      leave_sending_mode();
    break;
  case ScrewDriver::Mode::IDLE:
    run_idle_mode();
    leave_idle_mode();
    break;
  }
}
