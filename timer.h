#ifndef TIMER
#define TIMER

#include "raylib.h"
#include <stdlib.h>

typedef struct {
  double lifeTime;
  double startTime;
} Timer;

void startTimer(Timer *timer, double lifeTime) {
  timer->startTime = GetTime();
  timer->lifeTime = lifeTime;
}

bool isTimerDone(Timer *timer) {
  if (GetTime() - timer->startTime >= timer->lifeTime)
    return true;

  return false;
}

#endif
