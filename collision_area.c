#include "raylib.h"
#include "collision_area.h"

struct CircleCollisionArea NewCollisionArea(int centerX, int centerY, float radius, Color color) {
  struct CircleCollisionArea collisionArea = {centerX, centerY, radius, color};

  return collisionArea;
}

struct CircleCollisionArea UpdateCollisionAreaPosition(Vector2 position) {
  struct CircleCollisionArea collisionArea = { position.x, position.y };
  return collisionArea;
}
