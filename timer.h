#ifndef TIMER
#define TIMER
#include "raylib.h"

class Timer
{
public:
    double startTime; // Start time (seconds)
    double lifeTime;  // Lifetime (seconds)
    bool started = false;

    Timer(double lifetime)
    {
        this->lifeTime = lifetime;
    }

    void start() {
        startTime = GetTime();
        started = true;
    }

    void reset(double lifetime)
    {
        this->startTime = 0.0;
        this->lifeTime = lifetime;
        this->started = false;
    }

    bool isDone()
    {
        return GetTime() - startTime >= lifeTime;
    }

    double getElapsed()
    {
        return GetTime() - startTime;
    }
};
#endif
