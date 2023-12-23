typedef struct Timer {
    float lifeTime;    // Lifetime (seconds)
} Timer;

void StartTimer(Timer *timer, float lifetime);

void UpdateTimer(Timer* timer);

bool TimerDone(Timer timer);
