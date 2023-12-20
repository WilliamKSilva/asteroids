#include "raylib.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

struct Circle {
  int centerX;
  int centerY;
  float radius;
  Color color;
};

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);
  
  while (!WindowShouldClose())
  {
    BeginDrawing();
      ClearBackground(BLACK);
    EndDrawing();
  }

  CloseWindow();
    
  return 0;
}
