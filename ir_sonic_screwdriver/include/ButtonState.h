#ifndef BUTTON_STATE_H
#define BUTTON_STATE_H

class ButtonState
{
public:
    volatile int CurrentState;
    volatile int PreviousState;
    volatile int PressCount;
    volatile unsigned long LastPressTime;
    volatile unsigned long LastReleaseTime;

    ButtonState();
    void Reset();
    void ResetTime();
    void ResetPressCount();
    void StoreValue();
};

#endif // BUTTON_STATE_H
