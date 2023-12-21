#include "raylib.h"
#include <stdlib.h>

struct CircleCollisionArea {
  int centerX;
  int centerY;
  float radius;
  Color color;
};

struct CircleCollisionArea NewCollisionArea(int centerX, int centerY, float radius, Color color);
