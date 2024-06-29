#ifndef TIMER
#define TIMER
#include "raylib.h"

class Timer
{
public:
    double startTime; // Start time (seconds)
    double lifeTime;  // Lifetime (seconds)
    bool started = false;

    Timer(double lifetime);
    void start();
    void reset(double lifetime);
    bool isDone();
    double getElapsed();
};
#endif
