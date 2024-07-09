#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "raylib.h"
#include "string.h"

#include "projectile.h"
#include "timer.h"
#include "texture_pro.h"
#include "common.h"
#include "player.h"
#include "array.h"
#include "asteroid.h"
#include "enemy.h"

const float projectile_player_speed = 15.0;
const float projectile_enemy_speed = 10.0;

const float asteroid_speed = 4.0;
const float enemy_speed = 5.0;

// TODO: add menu
typedef struct {
  Player player;
  Array projectiles;
  Array asteroids;
  Array enemies;
  Timer asteroid_spawn_timer;
  Timer enemy_spawn_timer;
  Sounds sounds;
  StaticAssets assets;
  GameStatus game_status;
  GameMode game_mode;
} GameState;

void reset_state(GameState *state)
{
  // Reset player attributes 
  state->player.texture.dest.x = GetScreenWidth() / 2.0;
  state->player.texture.dest.y = GetScreenHeight() / 2.0;
  state->player.score = 0;
  state->player.lifes = 2;

  free(state->projectiles.ptr);
  state->projectiles.ptr = NULL;
  state->projectiles.length = 0;

  free(state->asteroids.ptr);
  state->asteroids.ptr = NULL;
  state->asteroids.length = 0;

  free(state->enemies.ptr);
  state->enemies.ptr = NULL;
  state->enemies.ptr = 0;
}

void on_player_death(GameState *state, CollidedObject object, int object_index)
{
  if (state->player.lifes == 1) {
    PlaySound(state->sounds.explode);
    printf("GAME: game over\n");
    state->game_status = GAME_OVER;
    state->player.death_position.x = state->player.texture.dest.x;
    state->player.death_position.y = state->player.texture.dest.y;
    timer_start(&state->player.death_timer, 2.0);
    reset_state(state);
    return;
  }

  state->player.lifes--;
  state->player.death_position.x = state->player.texture.dest.x;
  state->player.death_position.y = state->player.texture.dest.y;
  state->player.texture.dest.x = GetScreenWidth() / 2.0;
  state->player.texture.dest.y = GetScreenHeight() / 2.0;
  state->game_status = PLAYER_DEATH_PAUSE;
  timer_start(&state->player.death_timer, 3.0);

  if (object == ASTEROID) {
    delete_from_array(&state->asteroids, object_index);
    PlaySound(state->sounds.explode);
    return;
  }

  if (object == PROJECTILE) {
    delete_from_array(&state->projectiles, object_index);
    PlaySound(state->sounds.explode);
    return;
  }

  if (object == ENEMY) {
    delete_from_array(&state->enemies, object_index);
    PlaySound(state->sounds.explode);
    return;
  }
}

void update(GameState *state)
{
  if (state->game_status == PLAYER_DEATH_PAUSE) {
    if (timer_is_done(&state->player.death_timer)) {
      state->game_status = RUNNING;
    }

    return;
  }

  // Input updates
  move_player(&state->player, state->sounds.thrust);
  if (IsKeyPressed(KEY_SPACE)) {
    Vector2 player_pos = {
      .x = state->player.texture.dest.x,
      .y = state->player.texture.dest.y,
    };

    Vector2 projectile_pos = projectile_start_position(player_pos, state->player.texture.rotation);
    projectile_spawn(&state->projectiles, &projectile_pos, state->player.texture.rotation, false);
    PlaySound(state->sounds.shoot);
  }

  // Scripted updates
  // Projectiles
  for (int p = 0; p < state->projectiles.length; ++p) {
    Projectile* projectiles_data = state->projectiles.ptr;
    Projectile projectile = projectiles_data[p];

    // Collision with player
    if (object_collision_check(projectile.texture.dest, state->player.texture.dest) && projectile.is_enemy_projectile) {
      on_player_death(
        state,
        p,
        PROJECTILE
      );
      break;
    }

    if (projectile.is_enemy_projectile) {
      projectile_move(&projectiles_data[p], projectile_enemy_speed); 
    } else {
      projectile_move(&projectiles_data[p], projectile_player_speed); 
    }

    if (object_is_out_of_bounds(object_position(projectile.texture))) {
      delete_from_array(&state->projectiles, p);
    }
  }

  // Asteroids
  for (int a = 0; a < state->asteroids.length; ++a) {
    Asteroid *asteroids_data = (Asteroid*)state->asteroids.ptr;
    Asteroid *asteroid = &asteroids_data[a];
    Asteroid asteroid_copy = *asteroid;

    // Collision with player
    if (object_collision_check(asteroid->texture.dest, state->player.texture.dest)) {
      on_player_death(
        state, 
        a,
        ASTEROID
      ); 
      break;
    }

    bool collided_with_projectile = false;
    for (int p = 0; p < state->projectiles.length; ++p) {
      Projectile *projectiles_data = state->projectiles.ptr;
      Projectile *projectile = &projectiles_data[p];

      if (object_collision_check(projectile->texture.dest, asteroid->texture.dest)) {
        delete_from_array(&state->asteroids, a);
        delete_from_array(&state->projectiles, p);

        state->player.score += score.big_asteroid;
        PlaySound(state->sounds.destroyed);
        collided_with_projectile = true;
        break;
      }
    }

    if (collided_with_projectile && asteroid_copy.size == BIG) {
      Asteroid left = asteroid_build_small(asteroid_copy, DIAGONAL_LEFT);
      Asteroid right = asteroid_build_small(asteroid_copy, DIAGONAL_RIGHT);
      asteroid_spawn(&state->asteroids, left);
      asteroid_spawn(&state->asteroids, right);
      continue;
    }

    Vector2 position = move_object_by_spawn(
      object_position(asteroid->texture),
      asteroid->spawn,
      asteroid_speed,
      asteroid->diagonal_movement
    );

    asteroid->texture.dest.x = position.x;
    asteroid->texture.dest.y = position.y;

    if (object_is_out_of_bounds(position))
    {
      delete_from_array(&state->asteroids, a);
      continue;
    }
  }

  // Enemies 
  for (int e = 0; e < state->enemies.length; ++e) {
    Enemy *enemiesData = (Enemy*)state->enemies.ptr;
    Enemy *enemy = &enemiesData[e];

    if (object_is_out_of_bounds(object_position(enemy->texture)))
    {
      delete_from_array(&state->enemies, e);
      continue;
    }

    // Collision with player
    if (object_collision_check(enemy->texture.dest, state->player.texture.dest)) {
      on_player_death(
        state, 
        e,
        ENEMY 
      ); 
      break;
    }

    bool collided_with_projectile = false;
    for (int p = 0; p < state->projectiles.length; ++p) {
      Projectile *projectileData = state->projectiles.ptr;
      Projectile *projectile = &projectileData[p];

      if (projectile->is_enemy_projectile)
        continue;

      if (object_collision_check(projectile->texture.dest, enemy->texture.dest)) {
        delete_from_array(&state->enemies, e);
        delete_from_array(&state->projectiles, p);

        state->player.score += score.enemy;
        PlaySound(state->sounds.destroyed);
        collided_with_projectile = true;
        break;
      }
    }

    if (collided_with_projectile) 
      continue;

    enemy_move(enemy);
    enemy->texture.rotation = object_rotation_torwards_target(object_position(enemy->texture), object_position(state->player.texture));

    if (timer_is_done(&enemy->shoot_timer) && state->game_mode == REGULAR) {
      Vector2 enemy_pos = object_position(enemy->texture);
      Vector2 projectile_pos = projectile_start_position(enemy_pos, enemy->texture.rotation);

      projectile_spawn(
        &state->projectiles,
        &projectile_pos,
        enemy->texture.rotation,
        true
      );

      timer_start(&enemy->shoot_timer, enemy->shoot_timer.life_time);
    }
  }

  if (timer_is_done(&state->asteroid_spawn_timer)) {
    // Spawn new Asteroid 
    Asteroid asteroid = asteroid_build_big();
    asteroid_spawn(&state->asteroids, asteroid);
    
    // Restart timer
    timer_start(&state->asteroid_spawn_timer, state->asteroid_spawn_timer.life_time);
  }

  if (timer_is_done(&state->enemy_spawn_timer)) {
    // Spawn new Enemy 
    enemy_spawn(&state->enemies);
    
    // Restart timer
    timer_start(&state->enemy_spawn_timer, state->enemy_spawn_timer.life_time);
  }
}

void render(GameState *state)
{
  render_player(&state->player, &state->game_status, &state->assets.playerShipFire, &state->assets.damage);

  char lifeBuf[20];
  char scoreBuf[20];

  snprintf(scoreBuf, 20, "Score: %d", state->player.score);
  DrawText(scoreBuf, 50, 50, 25, WHITE);

  snprintf(lifeBuf, 20, "Lifes: %d", state->player.lifes);
  DrawText(lifeBuf, 50, 80, 25, RED);

  for (int i = 0; i < state->projectiles.length; ++i) {
    Projectile* ptr = (Projectile*)state->projectiles.ptr;
    render_texture_pro(ptr[i].texture); 
  }

  for (int i = 0; i < state->asteroids.length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)state->asteroids.ptr;
    render_texture_pro(asteroidsData[i].texture); 
  }

  for (int i = 0; i < state->enemies.length; ++i) {
    Enemy* enemiesData = (Enemy*)state->enemies.ptr;
    render_texture_pro(enemiesData[i].texture); 
  }
}

void render_game_over() {
  DrawText(
    "Game Over - Press space to restart",
    GetScreenWidth() / 2,
    GetScreenHeight() / 2,
    31,
    WHITE
  );
}

int main(int argc, char *argv[]) {
  InitWindow(1920, 1080, "Asteroids");
  InitAudioDevice();
  SetTargetFPS(60);
  srand(time(NULL));

  GameMode game_mode = REGULAR;
  if (argc > 1) {
    if (strcmp(argv[1], "mode=PACIFIC") == 0) {
      game_mode = PACIFIC;
    };
  }

  GameState state = {
    .game_mode = game_mode,
    .game_status = RUNNING,
    .player = player_build(),
    .asteroid_spawn_timer = {
      .life_time = 1.0,
      .start_time = 0.0
    },
    .enemy_spawn_timer = {
      .life_time = 2.0,
      .start_time = 0.0
    },
    .enemies = {
      .ptr = NULL,
      .length = 0,
      .itemSize = sizeof(Enemy)
    },
    .asteroids = {
      .ptr = NULL,
      .length = 0,
      .itemSize = sizeof(Asteroid)
    },
    .projectiles = {
      .ptr = NULL,
      .length = 0,
      .itemSize = sizeof(Projectile)
    },
    .sounds = {
      .shoot = LoadSound("./assets/shoot.wav"),
      .explode = LoadSound("./assets/explode.wav"),
      .thrust = LoadSound("./assets/thrust.wav"),
      .destroyed = LoadSound("./assets/asteroid.wav")
    },
    .assets = {
      .playerShipFire = LoadTexture("./assets/fire.png"),
      .damage = LoadTexture("./assets/damage.png")
    },
  };

  timer_start(&state.asteroid_spawn_timer, 1.0);
  timer_start(&state.enemy_spawn_timer, 2.0);

  while (!WindowShouldClose()) {
    // Update logic 
    if (state.game_status == GAME_OVER && IsKeyPressed(KEY_SPACE)) {
      state.game_status = RUNNING;
    } else {
      update(&state);
    }

    // Render logic
    BeginDrawing();
      ClearBackground(BLACK);
      if (state.game_status == GAME_OVER) {
        render_game_over();
      } else {
        render(&state);
      }
    EndDrawing();
  }

  return 0;
}
