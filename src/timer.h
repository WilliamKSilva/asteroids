#ifndef TIMER_H
#define TIMER_H

#include "raylib.h"
#include <stdlib.h>

typedef struct {
  double life_time;
  double start_time;
} Timer;

void timer_start(Timer *timer, double life_time);
bool timer_is_done(Timer *timer);

#endif
