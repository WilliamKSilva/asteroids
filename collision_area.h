#ifndef _COLLISION_AREA_H_
#define _COLLISION_AREA_H_

#include "raylib.h"
#include <stdlib.h>

struct CircleCollisionArea {
  int centerX;
  int centerY;
  float radius;
  Color color;
};

struct CircleCollisionArea NewCollisionArea(int centerX, int centerY, float radius, Color color);
struct CircleCollisionArea UpdateCollisionAreaPosition(Vector2 position);

#endif
