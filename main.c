#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include "timer.h"

#define PLAYER_MAX_SPEED 10.0
#define PLAYER_IMPULSE_SPEED 0.6
#define PLAYER_DRAG_SPEED 0.2

typedef struct {
  int centerX;
  int centerY;
  float radius;
  Color color;
} Circle;

typedef struct {
  Rectangle sourceRec;
  Rectangle destRec;
  Vector2 origin;
  float rotation;
} TexturePro;

typedef struct {
  Vector2 position;
  Texture2D *sprite;
  TexturePro *texturePro;
} GameObject;

typedef struct {
  GameObject gameObject;
  float speed;
  Timer* impulseTimer;
} Player;

void RenderGameObject(GameObject gameObject);
void UpdateGameObjectPosition(GameObject *gameObject, Vector2 position);
void MovePlayer(Player *player);

int main() {
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Asteroids");
  SetTargetFPS(60);

  Texture2D playerSprite = LoadTexture("./assets/player.png");
  TexturePro texturePro = {
    {
      0,
      0,
      playerSprite.width,
      playerSprite.height 
    },
    {
      (float)screenWidth / 2.0,
      (float)screenHeight / 2.0,
      playerSprite.width,
      playerSprite.height 
    },
    {
      playerSprite.width / 2.0,
      playerSprite.height / 2.0
    },
    0
  };

  Timer timer = {
    0.0,
    0.0,
    false
  };

  Player player = {
    {
      {
        (float)screenWidth / 2.0,
        (float)screenHeight / 2.0
      },
      &playerSprite,
      &texturePro
    },
    0.0,
    &timer
  };

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update should be framerate independent
    MovePlayer(&player);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    ClearBackground(BLACK);
    RenderGameObject(player.gameObject);
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
  return 0;
}

void RenderGameObject(GameObject gameObject) {
  if (gameObject.texturePro) {
    DrawTexturePro(
      *gameObject.sprite,
      gameObject.texturePro->sourceRec,
      gameObject.texturePro->destRec,
      gameObject.texturePro->origin,
      gameObject.texturePro->rotation,
      WHITE
    );

    return;
  }

  DrawTexture(*gameObject.sprite, gameObject.position.x, gameObject.position.y, WHITE);
}

void UpdateGameObjectPosition(GameObject *gameObject, Vector2 position) {
  gameObject->position = position;

  if (gameObject->texturePro) {
    gameObject->texturePro->destRec.x = gameObject->position.x;
    gameObject->texturePro->destRec.y = gameObject->position.y;
    return;
  }
}

void MovePlayer(Player *player) {
  if (player->gameObject.texturePro == NULL) {
    printf("%s\n", "Missing texture pro attributes");
    return;
  }

  if (IsKeyDown(KEY_A)) {
    player->gameObject.texturePro->rotation -= 10;
  }

  if (IsKeyDown(KEY_D)) {
    player->gameObject.texturePro->rotation += 10;
  }

  if (IsKeyDown(KEY_W)) {
    if (player->speed < PLAYER_MAX_SPEED) {
      player->speed += PLAYER_IMPULSE_SPEED;
    }

    Vector2 newPosition = player->gameObject.position;
    newPosition.x += (sin(player->gameObject.texturePro->rotation * DEG2RAD) * player->speed);
    newPosition.y -= (cos(player->gameObject.texturePro->rotation * DEG2RAD) * player->speed);
    UpdateGameObjectPosition(&player->gameObject, newPosition);
  }

  if (IsKeyUp(KEY_W)) {
    if (player->speed > 0)
      player->speed -= PLAYER_DRAG_SPEED;

    if (player->speed < 0) {
      player->speed = 0.0;
      return;
    }

    Vector2 newPosition = player->gameObject.position;
    newPosition.x += (sin(player->gameObject.texturePro->rotation * DEG2RAD) * player->speed);
    newPosition.y -= (cos(player->gameObject.texturePro->rotation * DEG2RAD) * player->speed);
    UpdateGameObjectPosition(&player->gameObject, newPosition);
  }
}
