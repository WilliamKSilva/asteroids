#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "array.h"
#include "texture_pro.h"
#include "timer.h"

typedef struct {
  TexturePro texture;
  float speed;
  int lifes;
  int score;
  bool is_boosting;
  Vector2 death_position;
  Timer death_timer;
} Player;

void move_player(Player *player, Sound thrustSound);
void render_player(Player *player, GameStatus *gameStatus, Texture2D *fireSprite, Texture2D *damageSprite);
void on_player_death(
  Player *player,
  Array *asteroids,
  Array *projectiles,
  Array *enemies,
  Sounds sounds,
  GameStatus *gameStatus,
  int indexOfElementCollided,
  CollidedObject collidedObject);
#endif
