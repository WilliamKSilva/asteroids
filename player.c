#include <raylib.h>
#include <stdlib.h>
#include "player.h"

struct Player* NewPlayer(Vector2 position, Texture2D sprite) {
  struct Player *player = malloc(sizeof(struct Player));
  player->position = position;
  player->sprite = sprite;

  return player;
}

void RenderPlayer(struct Player *player) {
  DrawCircle(player->collisionArea.centerX, player->collisionArea.centerY, player->collisionArea.radius, player->collisionArea.color);
  DrawTexture(player->sprite, player->position.x, player->position.y, WHITE);
}
