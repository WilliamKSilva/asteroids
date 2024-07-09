#include "player.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include "math.h"
#include "timer.h"

const float player_rotation_speed = 4.0;
const float player_impulse = 1.0;
const float player_max_impulse = 10.0;
const float player_drag = 0.5;

Player player_build()
{
  Vector2 pos = {
    .x = GetScreenWidth() / 2.0f,
    .y = GetScreenHeight() / 2.0f,
  };

  Player player = {
    .texture = build_texture_pro(&pos, "./assets/player.png", NULL),
    .speed = 0.0,
    .lifes = 2,
    .score = 0,
    .is_boosting = false,
  };

  return player;
}

void move_player(Player *player, Sound thrust_sound)
{
  if (IsKeyDown(KEY_A))
    player->texture.rotation -= player_rotation_speed;

  if (IsKeyDown(KEY_D))
    player->texture.rotation += player_rotation_speed;

  if (IsKeyDown(KEY_W)) {
    if (!IsSoundPlaying(thrust_sound))
      PlaySound(thrust_sound);

    if (player->speed <= player_max_impulse)
      player->speed += player_impulse;

    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
    player->is_boosting = true;
  }

  if (IsKeyUp(KEY_W)) {
    player->is_boosting = false;
    bool is_draggin_backwards = player->speed - player_drag < 0; 

    if (is_draggin_backwards) {
      player->speed = 0.0;
      player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
      player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
      return;
    }

    player->speed -= player_drag;
    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
  }
}

void render_player(
  Player *player,
  GameStatus *game_status,
  Texture2D *fire_sprite,
  Texture2D *damage_sprite) {
  if (*game_status == PLAYER_DEATH_PAUSE)
{
    TexturePro damage = build_texture_pro(&player->death_position, NULL, damage_sprite);
    render_texture_pro(damage);
    return;
  }

  render_texture_pro(player->texture);
  if (player->is_boosting) {
    render_texture_pro(build_fire_effect_texture(player->texture, fire_sprite));
  }
}

void on_player_death(
  Player *player,
  Array *asteroids,
  Array *projectiles,
  Array *enemies,
  Sounds sounds,
  GameStatus *game_status,
  int index_of_collided_element,
  CollidedObject collided_object)
{
  if (player->lifes == 1) {
    PlaySound(sounds.explode);
    printf("GAME: game over\n");
    *game_status = GAME_OVER;
    player->death_position.x = player->texture.dest.x;
    player->death_position.y = player->texture.dest.y;
    // TODO: find a better way to call this
    // reset_game_state(player, projectiles, asteroids, enemies);
    timer_start(&player->death_timer, 2.0);
    return;
  }

  player->lifes--;
  player->death_position.x = player->texture.dest.x;
  player->death_position.y = player->texture.dest.y;
  player->texture.dest.x = GetScreenWidth() / 2.0;
  player->texture.dest.y = GetScreenHeight() / 2.0;
  *game_status = PLAYER_DEATH_PAUSE;
  timer_start(&player->death_timer, 3.0);

  if (collided_object == ASTEROID) {
    delete_from_array(asteroids, index_of_collided_element);
    PlaySound(sounds.explode);
    return;
  }

  if (collided_object == PROJECTILE) {
    delete_from_array(projectiles, index_of_collided_element);
    PlaySound(sounds.explode);
    return;
  }

  if (collided_object == ENEMY) {
    delete_from_array(enemies, index_of_collided_element);
    PlaySound(sounds.explode);
    return;
  }
}
