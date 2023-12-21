#include "raylib.h"
#include "collision_area.h"

struct Player {
  struct CircleCollisionArea collisionArea;
  Vector2 position;
  Texture2D sprite;
};

struct Player* NewPlayer(Vector2 position, Texture2D sprite);
void RenderPlayer(struct Player *player);
