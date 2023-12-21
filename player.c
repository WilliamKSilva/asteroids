#include "raylib.h" 
#include "raymath.h"
#include <stdlib.h>
#include "collision_area.h"
#include "player.h"

struct Player* NewPlayer(Vector2 position, Vector2 speed, Texture2D sprite) {
  struct Player *player = malloc(sizeof(struct Player));
  player->position = position;
  player->sprite = sprite;
  player->speed = speed;

  return player;
}

void RenderPlayer(struct Player *player) {
  DrawCircle(player->collisionArea.centerX, player->collisionArea.centerY, player->collisionArea.radius, player->collisionArea.color);
  DrawTexture(player->sprite, player->position.x, player->position.y, WHITE);
}

void MovePlayer(struct Player *player) {
  Vector2 velocity = Vector2Zero();
  velocity = player->position;

  if (IsKeyDown(KEY_W)) {
   velocity.y -= player->speed.y;
  }

  if (IsKeyDown(KEY_S)) {
    velocity.y += player->speed.y;
  }

  if (IsKeyDown(KEY_A)) {
    velocity.x -= player->speed.x;
  }

  if (IsKeyDown(KEY_D)) {
    velocity.x += player->speed.x;
  }

  player->position = velocity;
  player->collisionArea = UpdateCollisionAreaPosition(player->position);
}
