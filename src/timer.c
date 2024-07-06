#include "timer.h"

void timer_start(Timer *timer, double lifeTime) {
  timer->startTime = GetTime();
  timer->lifeTime = lifeTime;
}

bool timer_is_done(Timer *timer) {
  if (GetTime() - timer->startTime >= timer->lifeTime)
    return true;

  return false;
}
