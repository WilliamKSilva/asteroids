#include "raylib.h"
#include "collision_area.h"

struct Player {
  struct CircleCollisionArea collisionArea;
  Vector2 position;
  Texture2D sprite;
  Vector2 speed;
};

struct Player* NewPlayer(Vector2 position, Vector2 speed, Texture2D sprite);
void RenderPlayer(struct Player *player);
void MovePlayer(struct Player *player);
