#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "raylib.h"

#include "projectile.h"
#include "timer.h"
#include "texture_pro.h"
#include "common.h"
#include "player.h"
#include "array.h"
#include "asteroid.h"
#include "enemy.h"

const int asteroid_big_score = 20;
const int enemy_score = 10;

const float projectile_player_speed = 15.0;
const float projectile_enemy_speed = 10.0;

const float asteroid_speed = 4.0;
const float enemy_speed = 5.0;

// TODO: finish code style related refactors (snake_case, functions brackets, etc)
// TODO: add smaller asteroids spawn
// TODO: add proper score system
// TODO: add menu
// TODO: add global game state struct

void reset(Player *player, Array *projectiles, Array *asteroids, Array *enemies)
{
  // Reset player attributes 
  player->texture.dest.x = GetScreenWidth() / 2.0;
  player->texture.dest.y = GetScreenHeight() / 2.0;
  player->score = 0;
  player->lifes = 2;

  free(projectiles->ptr);
  projectiles->ptr = NULL;
  projectiles->length = 0;

  free(asteroids->ptr);
  asteroids->ptr = NULL;
  asteroids->length = 0;

  free(enemies->ptr);
  enemies->ptr = NULL;
  enemies->length = 0;
}

void update(
  Player *player,
  Array *projectiles,
  Array *asteroids,
  Array *enemies,
  Timer *asteroid_spawn_timer,
  Timer *enemy_spawn_timer,
  Sounds sounds,
  GameStatus *game_status)
{
  if (*game_status == PLAYER_DEATH_PAUSE) {
    if (timer_is_done(&player->death_timer)) {
      *game_status = RUNNING;
    }

    return;
  }

  // Input updates
  move_player(player, sounds.thrust);
  if (IsKeyPressed(KEY_SPACE)) {
    Vector2 player_pos = {
      .x = player->texture.dest.x,
      .y = player->texture.dest.y,
    };

    Vector2 projectile_pos = projectile_start_position(player_pos, player->texture.rotation);
    projectile_spawn(projectiles, &projectile_pos, player->texture.rotation, false);
    PlaySound(sounds.shoot);
  }

  // Scripted updates
  // Projectiles
  for (int p = 0; p < projectiles->length; ++p) {
    Projectile* projectiles_data = projectiles->ptr;
    Projectile projectile = projectiles_data[p];

    // Collision with player
    if (object_collision_check(projectile.texture.dest, player->texture.dest) && projectile.is_enemy_projectile) {
      on_player_death(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        game_status,
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
      delete_from_array(projectiles, p);
    }
  }

  // Asteroids
  for (int a = 0; a < asteroids->length; ++a) {
    Asteroid *asteroids_data = (Asteroid*)asteroids->ptr;
    Asteroid *asteroid = &asteroids_data[a];

    // Collision with player
    if (object_collision_check(asteroid->texture.dest, player->texture.dest)) {
      on_player_death(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        game_status,
        a,
        ASTEROID
      ); 
      break;
    }

    bool collided_with_projectile = false;
    for (int p = 0; p < projectiles->length; ++p) {
      Projectile *projectiles_data = projectiles->ptr;
      Projectile *projectile = &projectiles_data[p];

      if (object_collision_check(projectile->texture.dest, asteroid->texture.dest)) {
        delete_from_array(asteroids, a);
        delete_from_array(projectiles, p);

        player->score += asteroid_big_score;
        PlaySound(sounds.destroyed);
        collided_with_projectile = true;
        break;
      }
    }

    if (collided_with_projectile) 
      continue;

    Vector2 position = move_object_by_spawn(
      object_position(asteroid->texture),
      asteroid->spawn,
      asteroid_speed,
      asteroid->diagonalMove
    );

    asteroid->texture.dest.x = position.x;
    asteroid->texture.dest.y = position.y;

    if (object_is_out_of_bounds(position))
    {
      delete_from_array(asteroids, a);
      continue;
    }
  }

  // Enemies 
  for (int e = 0; e < enemies->length; ++e) {
    Enemy *enemiesData = (Enemy*)enemies->ptr;
    Enemy *enemy = &enemiesData[e];

    if (object_is_out_of_bounds(object_position(enemy->texture)))
    {
      delete_from_array(enemies, e);
      continue;
    }

    // Collision with player
    if (object_collision_check(enemy->texture.dest, player->texture.dest)) {
      on_player_death(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        game_status,
        e,
        ENEMY 
      ); 
      break;
    }

    bool collided_with_projectile = false;
    for (int p = 0; p < projectiles->length; ++p) {
      Projectile *projectileData = projectiles->ptr;
      Projectile *projectile = &projectileData[p];

      if (projectile->is_enemy_projectile)
        continue;

      if (object_collision_check(projectile->texture.dest, enemy->texture.dest)) {
        delete_from_array(enemies, e);
        delete_from_array(projectiles, p);

        player->score += enemy_score;
        PlaySound(sounds.destroyed);
        collided_with_projectile = true;
        break;
      }
    }

    if (collided_with_projectile) 
      continue;

    enemy_move(enemy);
    enemy->texture.rotation = object_rotation_torwards_target(object_position(enemy->texture), object_position(player->texture));

    if (timer_is_done(&enemy->shootTimer)) {
      Vector2 enemy_pos = object_position(enemy->texture);
      Vector2 projectile_pos = projectile_start_position(enemy_pos, enemy->texture.rotation);

      projectile_spawn(
        projectiles,
        &projectile_pos,
        enemy->texture.rotation,
        true
      );

      timer_start(&enemy->shootTimer, enemy->shootTimer.lifeTime);
    }
  }

  if (timer_is_done(asteroid_spawn_timer)) {
    // Spawn new Asteroid 
    asteroid_spawn(asteroids);
    
    // Restart timer
    timer_start(asteroid_spawn_timer, asteroid_spawn_timer->lifeTime);
  }

  if (timer_is_done(enemy_spawn_timer)) {
    // Spawn new Enemy 
    enemy_spawn(enemies);
    
    // Restart timer
    timer_start(enemy_spawn_timer, enemy_spawn_timer->lifeTime);
  }
}

void render(
  Player *player,
  Array projectiles,
  Array asteroids,
  Array enemies,
  GameStatus *gameStatus,
  StaticAssets assets)
{
  render_player(player, gameStatus, &assets.playerShipFire, &assets.damage);

  char lifeBuf[20];
  char scoreBuf[20];

  snprintf(scoreBuf, 20, "Score: %d", player->score);
  DrawText(scoreBuf, 50, 50, 25, WHITE);

  snprintf(lifeBuf, 20, "Lifes: %d", player->lifes);
  DrawText(lifeBuf, 50, 80, 25, RED);

  for (int i = 0; i < projectiles.length; ++i) {
    Projectile* ptr = (Projectile*)projectiles.ptr;
    render_texture_pro(ptr[i].texture); 
  }

  for (int i = 0; i < asteroids.length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)asteroids.ptr;
    render_texture_pro(asteroidsData[i].texture); 
  }

  for (int i = 0; i < enemies.length; ++i) {
    Enemy* enemiesData = (Enemy*)enemies.ptr;
    render_texture_pro(enemiesData[i].texture); 
  }
}

void renderGameOver() {
  DrawText(
    "Game Over - Press space to restart",
    GetScreenWidth() / 2,
    GetScreenHeight() / 2,
    31,
    WHITE
  );
}

int main() {
  InitWindow(1920, 1080, "Asteroids");
  InitAudioDevice();
  SetTargetFPS(60);
  srand(time(NULL));

  GameStatus gameStatus = RUNNING; 

  Sounds sounds = {
    .shoot = LoadSound("./assets/shoot.wav"),
    .explode = LoadSound("./assets/explode.wav"),
    .thrust = LoadSound("./assets/thrust.wav"),
    .destroyed = LoadSound("./assets/asteroid.wav")
  };

  StaticAssets assets = {
    .playerShipFire = LoadTexture("./assets/fire.png"),
    .damage = LoadTexture("./assets/damage.png")
  };

  Vector2 playerStartPosition = { 
    .x = (float)GetScreenWidth() / 2.0,
    .y = (float)GetScreenHeight() / 2.0
  };
  Player player = {
    .texture = build_texture_pro(&playerStartPosition, "./assets/player.png", NULL),
    .speed = 0.0,
    .lifes = 2,
    .score = 0,
    .is_boosting = false,
  };

  Timer asteroidSpawnTimer;
  timer_start(&asteroidSpawnTimer, 2.0);

  Timer enemySpawnTimer;
  timer_start(&enemySpawnTimer, 2.0);

  Array projectiles = {
    .ptr = NULL,
    .length = 0,
    .itemSize = sizeof(Projectile)
  };

  Array asteroids = {
    .ptr = NULL,
    .length = 0,
    .itemSize = sizeof(Asteroid)
  };

  Array enemies = {
    .ptr = NULL,
    .length = 0,
    .itemSize = sizeof(Enemy)
  };

  while (!WindowShouldClose()) {
    // Update logic 
    if (gameStatus == GAME_OVER && IsKeyPressed(KEY_SPACE)) {
      gameStatus = RUNNING;
    } else {
      update(
        &player,
        &projectiles,
        &asteroids,
        &enemies,
        &asteroidSpawnTimer,
        &enemySpawnTimer,
        sounds,
        &gameStatus
      );
    }

    // Render logic
    BeginDrawing();
      ClearBackground(BLACK);
      if (gameStatus == GAME_OVER) {
        renderGameOver();
      } else {
        render(&player, projectiles, asteroids, enemies, &gameStatus, assets);
      }
    EndDrawing();
  }

  return 0;
}
