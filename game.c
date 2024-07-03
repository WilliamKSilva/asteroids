#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

#define PLAYER_ROTATION_SPEED 4.0
#define PLAYER_IMPULSE 1.0
#define PLAYER_MAX_IMPULSE 10.0
#define PLAYER_DRAG 0.5 

// TODO: draw my own assets

typedef struct {
  Texture2D sprite;
  Rectangle source;
  Rectangle dest;
  Vector2 origin;
  float rotation;
  Color color;
} TexturePro;

typedef struct {
  TexturePro texture;
  float speed;
} Player;

TexturePro buildTexturePro(Vector2 startPosition, const char *spritePath) {
  Texture2D sprite = LoadTexture(spritePath);
  TexturePro texture = {
    .color = WHITE,
    .source = {
      .x = 0.0,
      .y = 0.0,
      .width = (float)sprite.width,
      .height = (float)sprite.height
    },
    .dest = {
      .x = startPosition.x,
      .y = startPosition.y,
      .width = (float)sprite.width,
      .height = (float)sprite.height
    },
    .sprite = sprite,
    .origin = {
      .x =  (float)sprite.width / 2.0,
      .y = (float)sprite.height / 2.0
    },
    .rotation = 0.0
  };

  return texture;
}

void renderTexturePro(TexturePro texture) {
  DrawTexturePro(
    texture.sprite,
    texture.source,
    texture.dest,
    texture.origin,
    texture.rotation,
    texture.color 
  );
}

void movePlayer(Player *player) {
  if (IsKeyDown(KEY_A))
    player->texture.rotation -= PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_D))
    player->texture.rotation += PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_W)) {
    if (player->speed <= PLAYER_MAX_IMPULSE)
      player->speed += PLAYER_IMPULSE;

    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
  }

  if (IsKeyUp(KEY_W)) {
    bool isDraggingBackwards = player->speed - PLAYER_DRAG < 0; 

    if (isDraggingBackwards) {
      player->speed = 0.0;
      player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
      player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
      return;
    }

    player->speed -= PLAYER_DRAG;
    player->texture.dest.x += sin(player->texture.rotation * DEG2RAD) * player->speed;
    player->texture.dest.y -= cos(player->texture.rotation * DEG2RAD) * player->speed;
  }
}

void update(Player *player) {
  // Input updates
  movePlayer(player);
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  Vector2 playerStartPosition = { 
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  Player player = {
    .texture = buildTexturePro(playerStartPosition, "./assets/player.png"),
    .speed = 0.0
  };

  while (!WindowShouldClose()) {
    update(&player);

    BeginDrawing(); 
      ClearBackground(BLACK);
      renderTexturePro(player.texture);
    EndDrawing();
  }

  return 0;
}
