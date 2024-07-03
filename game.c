#include "raylib.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

#define PLAYER_ROTATION_SPEED 2.0

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
    .texture = buildTexturePro(playerStartPosition, "./assets/player.png")
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
