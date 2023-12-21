#include "raylib.h"
#include "player.h"

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
  struct CircleCollisionArea collisionArea = NewCollisionArea(playerPosition.x, playerPosition.y, 50, BLACK);
  player->collisionArea = collisionArea;

  while (!WindowShouldClose())
  {
    MovePlayer(player);

    BeginDrawing();
      ClearBackground(BLACK);

      RenderPlayer(player);
    EndDrawing();
  }

  free(player);

  CloseWindow();
    
  return 0;
}
