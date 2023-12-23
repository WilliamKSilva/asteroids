#include "raylib.h"
#include <stdlib.h>

typedef struct Timer {
  double lifeTime; // Lifetime (seconds)
} Timer;

void StartTimer(Timer *timer, float lifetime) {
  if (timer != NULL) {
    timer->lifeTime = lifetime;
  }
}

void UpdateTimer(Timer *timer) {
    if (timer != NULL && timer->lifeTime > 0) {
        timer->lifeTime -= GetFrameTime();
    }
}

bool TimerDone(Timer *timer) {
    if (timer != NULL) {
        return timer->lifeTime <= 0;
    }

    return false;
}
