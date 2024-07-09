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

Player player_build();
void move_player(Player *player, Sound thrust_sound);
void render_player(Player *player, GameStatus *game_status, Texture2D *fire_sprite, Texture2D *damage_sprite);
void on_player_death(
  Player *player,
  Array *asteroids,
  Array *projectiles,
  Array *enemies,
  Sounds sounds,
  GameStatus *game_status,
  int index_of_collided_element,
  CollidedObject collided_object);
#endif
