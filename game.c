#include "raylib.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

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
} GameObject;

GameObject buildGameObject(Vector2 startPosition, const char *spritePath) {
  Texture2D sprite = LoadTexture(spritePath);
  TexturePro texture = {
    .color = WHITE,
    .source = {
      .x = 0.0,
      .y = 0.0,
      .width = sprite.width,
      .height = sprite.height
    },
    .dest = {
      .x = startPosition.x,
      .y = startPosition.y,
      .width = sprite.width,
      .height = sprite.height
    },
    .sprite = sprite,
    .origin = {
      .x =  sprite.width / 2.0,
      .y = sprite.height / 2.0
    },
    .rotation = 0.0
  };

  GameObject gameObject = {
    .texture = texture
  };

  return gameObject;
}

void renderGameObject(GameObject gameObject) {
  DrawTexturePro(
    gameObject.texture.sprite,
    gameObject.texture.source,
    gameObject.texture.dest,
    gameObject.texture.origin,
    gameObject.texture.rotation,
    gameObject.texture.color 
  );
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  Vector2 playerStartPosition = { 
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  GameObject player = buildGameObject(playerStartPosition, "./assets/player.png"); 

  while (!WindowShouldClose()) {
    BeginDrawing(); 
      renderGameObject(player);
    EndDrawing();
  }
}
