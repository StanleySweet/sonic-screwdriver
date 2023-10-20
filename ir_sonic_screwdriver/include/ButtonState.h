#ifndef BUTTON_STATE_H
#define BUTTON_STATE_H

struct ButtonState
{
    volatile int CurrentState;
    volatile int PreviousState;
    volatile int PressCount;
    volatile unsigned long LastPressTime;
    volatile unsigned long LastReleaseTime;
};

#endif // BUTTON_STATE_H
