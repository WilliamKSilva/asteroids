#include "timer.h"

void timer_start(Timer *timer, double life_time) 
{
  timer->start_time = GetTime();
  timer->life_time = life_time;
}

bool timer_is_done(Timer *timer)
{
  if (GetTime() - timer->start_time >= timer->life_time)
    return true;

  return false;
}