#include <stdio.h>
#include "raylib.h"
#include "collision_area.h"
#include "player.h"
#include "asteroid.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  char *path = "/home/williamkds/Projects/asteroids/assets/spaceship.png";
  Texture2D spaceshipSprite = LoadTexture(path);

  Vector2 playerPosition = {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
  Vector2 playerSpeed = {4.0f, 4.0f};
  struct Player *player = NewPlayer(playerPosition, playerSpeed, spaceshipSprite);
  struct CircleCollisionArea collisionArea = NewCollisionArea(playerPosition.x, playerPosition.y, 50, WHITE);
  player->collisionArea = collisionArea;

  // TODO: move Asteroid composition to independent function 
  char *asteroidPath = "/home/williamkds/Projects/asteroids/assets/meteors/meteor-big.png";
  Texture2D asteroidSprite = LoadTexture(asteroidPath);
  Vector2 asteroidPosition = {SCREEN_WIDTH / 2.0 + 200, SCREEN_HEIGHT / 2.0};
  Vector2 asteroidSpeed = {2.0f, 3.0f};
  struct Asteroid *asteroid = NewAsteroid(asteroidPosition, asteroidSpeed, asteroidSprite);
  struct CircleCollisionArea asteroidCollisionArea = NewCollisionArea(asteroidPosition.x, asteroidPosition.y, 50, BLACK);
  asteroid->collisionArea = asteroidCollisionArea;

  while (!WindowShouldClose())
  {
    MovePlayer(player);

    printf("%i", player->collisionArea.centerX);
    printf("\n");

    BeginDrawing();
      ClearBackground(BLACK);

      RenderPlayer(player);
      RenderAsteroid(asteroid);
    EndDrawing();
  }

  free(player);

  CloseWindow();
    
  return 0;
}
