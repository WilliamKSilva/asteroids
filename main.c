#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PLAYER_MAX_SPEED 10.0
#define PLAYER_IMPULSE_SPEED 0.6
#define PLAYER_DRAG_SPEED 0.2
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

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
} Player;

GameObject BuildGameObject(Vector2 position, const char* spritePath, bool texturePro);
Player BuildPlayer(GameObject gameObject);
void RenderGameObject(GameObject gameObject);
void UpdateGameObjectPosition(GameObject *gameObject, Vector2 position);
void MovePlayer(Player *player);

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  Vector2 playerPosition = {
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  GameObject gameObject = BuildGameObject(playerPosition, "./assets/player.png", true);
  Player player = BuildPlayer(gameObject); 

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

GameObject BuildGameObject(Vector2 position, const char* spritePath, bool texturePro) {
  GameObject gameObject;
  Texture2D sprite = LoadTexture(spritePath);
  gameObject.position = position;
  gameObject.sprite = &sprite;
  if (texturePro) {
    TexturePro* texturePro = malloc(sizeof(TexturePro));
    *texturePro = (TexturePro){
      .sourceRec = {
        .x = 0,
        .y = 0,
        .width = sprite.width,
        .height = sprite.height 
      },
      .destRec = {
        .x = position.x,
        .y = position.y,
        .width = sprite.width,
        .height = sprite.height 
      },
      .origin = {
        .x = sprite.width / 2.0,
        .y = sprite.height / 2.0
      },
      .rotation = 0
    };
    gameObject.texturePro = texturePro;
  }

  return gameObject;
}

Player BuildPlayer(GameObject gameObject) {
  Player player = {
    .gameObject = gameObject,
    .speed = 0.0
  };

  return player;
}

void MovePlayer(Player *player) {
  if (player->gameObject.texturePro == NULL) {
    printf("%s\n", "Missing texture pro attributes");
    return;
  }

  if (IsKeyDown(KEY_A)) {
    player->gameObject.texturePro->rotation -= 5;
  }

  if (IsKeyDown(KEY_D)) {
    player->gameObject.texturePro->rotation += 5;
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
