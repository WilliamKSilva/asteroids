#include "asteroid.h"
#include "collision_area.h"
#include "player.h"
#include "raylib.h"
#include "timer.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

bool OutOfScreen(Vector2 position) {
  return position.x >= 1920 || position.y >= 1080;
};

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  char cwd[PATH_MAX];
  char *path = getcwd(cwd, sizeof(cwd));
  char *spaceshipSpritePath = strcat(path, "/assets/spaceship.png");

  if (path == NULL) {
    perror("Error trying to load meteor asset");
  }

  Texture2D spaceshipSprite = LoadTexture(spaceshipSpritePath);

  Vector2 playerPosition = {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
  Vector2 playerSpeed = {4.0f, 4.0f};
  struct Player *player =
      NewPlayer(playerPosition, playerSpeed, spaceshipSprite);
  struct CircleCollisionArea collisionArea =
      NewCollisionArea(playerPosition.x, playerPosition.y, 50, WHITE);
  player->collisionArea = collisionArea;

  struct Asteroid *asteroids = AllocateInitialAsteroids();

  Timer asteroidsSpawnTimer = { 0 };

  StartTimer(&asteroidsSpawnTimer, 1.0);

  ToggleFullscreen();

  while (!WindowShouldClose()) {
    MovePlayer(player);

    for (int i = 0; i < 6; i++) {
      MoveAsteroid(&asteroids[i]);
    }

    UpdateTimer(&asteroidsSpawnTimer);

    if (TimerDone(asteroidsSpawnTimer)) {
      CreateAsteroids(asteroids);
      StartTimer(&asteroidsSpawnTimer, 5.0);
    }

    BeginDrawing();
    ClearBackground(BLACK);

    RenderPlayer(player);

    for (int i = 0; i < 6; i++) {
      RenderAsteroid(&asteroids[i]);
    }
    EndDrawing();
  }

  free(player);

  CloseWindow();

  return 0;
}
