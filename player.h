#ifndef _PLAYERH_
#define _PLAYERH_
#include "raylib.h"
#include "collision_area.h"

struct TexturePro {
  Texture2D image;
  Rectangle sourceRec;
  Rectangle destRec;
  Vector2 origin;
  int rotation;
  Color color;
};

struct Player {
  struct CircleCollisionArea collisionArea;
  Vector2 position;
  Vector2 speed;
  struct TexturePro sprite;
};

struct Player* NewPlayer(Vector2 position, Vector2 speed, Texture2D sprite);
void RenderPlayer(struct Player *player);
void MovePlayer(struct Player *player);
#endif
