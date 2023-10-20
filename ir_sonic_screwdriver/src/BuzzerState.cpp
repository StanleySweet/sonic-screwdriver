#include "../include/BuzzerState.h"

#include "../include/Constants.h"
#include "../include/Pitches.h"

#include <Arduino.h>

BuzzerState::BuzzerState()
{
    const int NOTE_DURATION_FACTOR = 275;
    this->LastToneTime = millis();
    this->ToneData = {
        SToneData{Pitches::Note_F7, NOTE_DURATION_FACTOR / 9},
        SToneData{Pitches::Note_G4, NOTE_DURATION_FACTOR / 12},
        SToneData{Pitches::Note_A2, NOTE_DURATION_FACTOR / 7}
    };
    this->m_ToneDuration = ToneData[0].Duration + ToneData[1].Duration + ToneData[2].Duration;
}

unsigned long BuzzerState::GetToneDuration() const
{
    return this->m_ToneDuration;
}

void BuzzerState::Play(uint8_t) const
{
    for (const SToneData& data : this->ToneData)
        tone(TONE_PIN, data.Frequency, data.Duration);
}
