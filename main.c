#include "raylib.h"
#include <stdio.h>

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

void RenderGameObject(GameObject gameObject);
void MovePlayer(GameObject *player);

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

  GameObject player = {
      {
        (float)screenWidth / 2.0,
        (float)screenHeight / 2.0
      },
      &playerSprite,
      &texturePro
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
    RenderGameObject(player);
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

void MovePlayer(GameObject *player) {
  if (player->texturePro == NULL) {
    return;
  }

  if (IsKeyDown(KEY_A)) {
    player->texturePro->rotation -= 10;
  }

  if (IsKeyDown(KEY_D)) {
    player->texturePro->rotation += 10;
  }
}
