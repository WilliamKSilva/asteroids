#include "timer.h"

Timer::Timer(double lifetime)
{
  this->lifeTime = lifetime;
}

void Timer::start()
{
  startTime = GetTime();
  started = true;
}

void Timer::reset(double lifetime)
{
  this->startTime = 0.0;
  this->lifeTime = lifetime;
  this->started = false;
}

bool Timer::isDone()
{
  return GetTime() - startTime >= lifeTime;
}

double Timer::getElapsed()
{
  return GetTime() - startTime;
}