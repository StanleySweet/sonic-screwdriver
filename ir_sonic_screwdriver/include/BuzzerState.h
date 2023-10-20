#ifndef BUZZER_STATE_H
#define BUZZER_STATE_H

#include <array>

struct SToneData
{
    unsigned int Frequency;
    unsigned long Duration;
};
class BuzzerState
{
public:
    BuzzerState();
    unsigned long GetToneDuration() const;
    void Play(uint8_t repeat = 0) const;
    std::array<SToneData, 3U> ToneData;
    unsigned long LastToneTime;

private:
    unsigned long m_ToneDuration;
};

#endif // BUZZER_STATE_H
