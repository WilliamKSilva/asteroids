#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

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

const float asteroid_speed = 5.0;
const float enemy_speed = 3.0;

// TODO: remove useless macros?

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
  Timer *asteroidSpawnTimer,
  Timer *enemySpawnTimer,
  Sounds sounds,
  GameStatus *gameStatus)
{
  if (*gameStatus == PLAYER_DEATH_PAUSE) {
    if (timer_is_done(&player->death_timer)) {
      *gameStatus = RUNNING;
    }

    return;
  }

  // Input updates
  move_player(player, sounds.thrust);
  if (IsKeyPressed(KEY_SPACE)) {
    Vector2 playerPosition = {
      .x = player->texture.dest.x,
      .y = player->texture.dest.y,
    };

    Vector2 projectilePos = projectile_start_position(playerPosition, player->texture.rotation);
    projectile_spawn(projectiles, &projectilePos, player->texture.rotation, false);
    PlaySound(sounds.shoot);
  }

  // Scripted updates
  // Projectiles
  for (int i = 0; i < projectiles->length; ++i) {
    Projectile* projectilesData = projectiles->ptr;
    Projectile projectile = projectilesData[i];

    if (object_collision_check(projectile.texture.dest, player->texture.dest) && projectile.enemyProjectile) {
      on_player_death(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        gameStatus,
        i,
        PROJECTILE
      );
      deleteFromArray(projectiles, i);
      break;
    }

    if (projectile.enemyProjectile)
      projectile_move(&projectilesData[i], projectile_enemy_speed); 
    else
      projectile_move(&projectilesData[i], projectile_player_speed); 

    Vector2 projectilePosition = {
      .x = projectile.texture.dest.x,
      .y = projectile.texture.dest.y
    };

    if (object_is_out_of_bounds(projectilePosition)) {
      deleteFromArray(projectiles, i);
    }
  }

  // Asteroids
  for (int i = 0; i < asteroids->length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)asteroids->ptr;
    Asteroid asteroid = asteroidsData[i];

    Vector2 currentPosition = {
      .x = asteroid.texture.dest.x,
      .y = asteroid.texture.dest.y
    };

    Vector2 position = move_object_by_spawn(currentPosition, asteroid.spawn, asteroid_speed, asteroid.diagonalMove);
    asteroidsData[i].texture.dest.x = position.x;
    asteroidsData[i].texture.dest.y = position.y;

    if (object_is_out_of_bounds(position))
    {
      deleteFromArray(asteroids, i);
      continue;
    }

    if (object_collision_check(asteroidsData[i].texture.dest, player->texture.dest)) {
      on_player_death(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        gameStatus,
        i,
        ASTEROID
      ); 
      break;
    }

    for (int j = 0; j < projectiles->length; ++j) {
      Projectile *projectileData = projectiles->ptr;

      if (object_collision_check(projectileData[j].texture.dest, asteroidsData[i].texture.dest)) {
        deleteFromArray(asteroids, i);
        deleteFromArray(projectiles, j);

        player->score += asteroid_big_score;
        PlaySound(sounds.asteroidDestroyed);
        break;
      }
    }
  }

  // Enemies
  for (int i = 0; i < enemies->length; ++i) {
    Enemy *enemiesData = (Enemy*)enemies->ptr;
    Enemy enemy = enemiesData[i];

    if (object_collision_check(enemy.texture.dest, player->texture.dest)) {
      on_player_death(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        gameStatus,
        i,
        ENEMY
      );
      break;
    }

    for (int j = 0; j < projectiles->length; ++j) {
      Projectile *projectileData = projectiles->ptr;

      if (object_collision_check(projectileData[j].texture.dest, enemiesData[i].texture.dest) && !projectileData[i].enemyProjectile) {
        deleteFromArray(enemies, i);
        deleteFromArray(projectiles, j);

        player->score += enemy_score;
        PlaySound(sounds.asteroidDestroyed);
        break;
      }
    }

    Vector2 pos = {
      .x = enemy.texture.dest.x,
      .y = enemy.texture.dest.y,
    };

    Vector2 playerPos = {
      .x = player->texture.dest.x,
      .y = player->texture.dest.y,
    };

    Vector2 direction = Vector2Subtract(playerPos, pos); 

    // X and Y needs to be swapped - (0, 0) = TOP LEFT CORNER
    // Negative Y because upwards is to the bottom direction in raylib
    float angle = atan2(direction.x, -direction.y);
    enemiesData[i].texture.rotation = angle * RAD2DEG;

    Vector2 position = move_object_by_spawn(
      pos,
      enemy.spawn,
      enemy_speed,
      false
    );
    enemiesData[i].texture.dest.x = position.x;
    enemiesData[i].texture.dest.y = position.y;

    if (object_is_out_of_bounds(position))
      deleteFromArray(enemies, i);

    if (timer_is_done(&enemiesData[i].shootTimer)) {
      Vector2 projectilePos = projectile_start_position(pos, enemy.texture.rotation);
      projectile_spawn(projectiles, &projectilePos, enemy.texture.rotation, true);
      timer_start(&enemiesData[i].shootTimer, 3.0);
      PlaySound(sounds.shoot);
    }
  }

  if (timer_is_done(asteroidSpawnTimer)) {
    // Spawn new Asteroid 
    asteroid_spawn(asteroids);
    
    // Restart timer
    timer_start(asteroidSpawnTimer, asteroidSpawnTimer->lifeTime);
  }

  if (timer_is_done(enemySpawnTimer)) {
    // Spawn new Enemy 
    enemy_spawn(enemies);
    
    // Restart timer
    timer_start(enemySpawnTimer, enemySpawnTimer->lifeTime);
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
    .asteroidDestroyed = LoadSound("./assets/asteroid.wav")
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
  timer_start(&enemySpawnTimer, 5.0);

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
