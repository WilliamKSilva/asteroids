#include "raylib.h"
#include "collision_area.h"

struct CircleCollisionArea NewCollisionArea(int centerX, int centerY, float radius, Color color) {
  struct CircleCollisionArea collisionArea = {centerX, centerY, radius, color};

  return collisionArea;
}
