#include "player.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "timer.h"

const float player_rotation_speed = 4.0;
const float player_impulse = 1.0;
const float player_max_impulse = 10.0;
const float player_drag = 0.5;

void move_player(Player *player, Sound thrustSound) {
  if (IsKeyDown(KEY_A))
    player->texture.rotation -= player_rotation_speed;

  if (IsKeyDown(KEY_D))
    player->texture.rotation += player_rotation_speed;

  if (IsKeyDown(KEY_W)) {
    if (!IsSoundPlaying(thrustSound))
      PlaySound(thrustSound);

    if (player->speed <= player_max_impulse)
      player->speed += player_impulse;

    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
    player->is_boosting = true;
  }

  if (IsKeyUp(KEY_W)) {
    player->is_boosting = false;
    bool isDraggingBackwards = player->speed - player_drag < 0; 

    if (isDraggingBackwards) {
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
  GameStatus *gameStatus,
  Texture2D *fireSprite,
  Texture2D *damageSprite) {
  if (*gameStatus == PLAYER_DEATH_PAUSE) {
    TexturePro damage = build_texture_pro(&player->death_position, NULL, damageSprite);
    render_texture_pro(damage);
    return;
  }

  render_texture_pro(player->texture);
  if (player->is_boosting) {
    render_texture_pro(build_fire_effect_texture(player->texture, fireSprite));
  }
}

void on_player_death(
  Player *player,
  Array *asteroids,
  Array *projectiles,
  Array *enemies,
  Sounds sounds,
  GameStatus *gameStatus,
  int indexOfElementCollided,
  CollidedObject collidedObject)
{
  if (player->lifes == 1) {
    PlaySound(sounds.explode);
    printf("GAME: game over\n");
    *gameStatus = GAME_OVER;
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
  *gameStatus = PLAYER_DEATH_PAUSE;
  timer_start(&player->death_timer, 3.0);

  if (collidedObject == ASTEROID) {
    deleteFromArray(asteroids, indexOfElementCollided);
    PlaySound(sounds.explode);
    return;
  }

  if (collidedObject == PROJECTILE) {
    deleteFromArray(projectiles, indexOfElementCollided);
    PlaySound(sounds.explode);
    return;
  }

  if (collidedObject == ENEMY) {
    deleteFromArray(enemies, indexOfElementCollided);
    PlaySound(sounds.explode);
    return;
  }
}
