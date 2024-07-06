#ifndef TIMER_H
#define TIMER_H

#include "raylib.h"
#include <stdlib.h>

typedef struct {
  double lifeTime;
  double startTime;
} Timer;

void timer_start(Timer *timer, double lifeTime);
bool timer_is_done(Timer *timer);

#endif
