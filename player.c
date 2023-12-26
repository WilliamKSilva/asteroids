#include "raylib.h" 
#include "raymath.h"
#include <stdlib.h>
#include "collision_area.h"
#include "player.h"

struct Player* NewPlayer(Vector2 position, Vector2 speed, Texture2D sprite) {
  struct Player *player = malloc(sizeof(struct Player));
  player->position = position;
  player->speed = speed;

  int frameWidth = sprite.width;
  int frameHeight = sprite.height;

  Rectangle sourceRec = { 0.0f, 0.0f, (float)frameWidth, (float)frameHeight };

  // Where on the screen we gone render the sprite
  Rectangle destRec = { player->position.x, player->position.y, (float)frameWidth, (float)frameHeight };

  Vector2 origin = { (float)frameWidth / 2, (float)frameHeight / 2 };
  int rotation = 0;

  struct TexturePro texture = {sprite, sourceRec, destRec, origin, rotation, WHITE};
  player->sprite = texture;

  return player;
}

void RenderPlayer(struct Player *player) {
  DrawCircle(player->collisionArea.centerX, player->collisionArea.centerY, player->collisionArea.radius, player->collisionArea.color);
  DrawTexturePro(player->sprite.image, player->sprite.sourceRec, player->sprite.destRec, player->sprite.origin, (float)player->sprite.rotation, player->sprite.color);
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

  if (IsKeyDown(KEY_D)) {
    player->sprite.rotation += 5;
  }

  if (IsKeyDown(KEY_A)) {
    player->sprite.rotation -= 5;
  }

  player->position = velocity;
  player->collisionArea = UpdateCollisionAreaPosition(player->position);
}
