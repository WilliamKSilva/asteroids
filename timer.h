#ifndef TIMER
#define TIMER
#include "raylib.h"

typedef struct Timer {
    double startTime;   // Start time (seconds)
    double lifeTime;    // Lifetime (seconds)
    bool started;
} Timer;

void StartTimer(Timer *timer, double lifetime)
{
    timer->startTime = GetTime();
    timer->lifeTime = lifetime;
    timer->started = true;
}

void ResetTimer(Timer *timer)
{
	timer->startTime = 0.0;
	timer->lifeTime = 0.0;
	timer->started = false;
}

bool TimerDone(Timer timer)
{
    return GetTime() - timer.startTime >= timer.lifeTime;
}

double GetElapsed(Timer timer)
{
    return GetTime() - timer.startTime;
}
#endif
