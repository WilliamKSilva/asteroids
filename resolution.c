#include "raylib.h"
#include "resolution.h"

bool IsObjectOnScreen(Vector2 position) {
  bool xAxis = position.x >= SCREEN_WIDTH || position.x <= 0;
  bool yAxis = position.y >= SCREEN_HEIGHT || position.y <= 0;

  return xAxis || yAxis;
}
