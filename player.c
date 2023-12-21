#include <raylib.h>
#include <stdlib.h>
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
  if (IsKeyDown(KEY_W)) {
    player->position.y -= player->speed.y;
  }

  if (IsKeyDown(KEY_S)) {
    player->position.y += player->speed.y;
  }

  if (IsKeyDown(KEY_A)) {
    player->position.x -= player->speed.x;
  }

  if (IsKeyDown(KEY_D)) {
    player->position.x += player->speed.x;
  }
}
