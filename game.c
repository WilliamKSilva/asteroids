#include "raylib.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGTH 1080 

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

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGTH, "Asteroids");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing(); 
      DrawText("hello world", SCREEN_WIDTH / 2, SCREEN_HEIGTH / 2, 31, WHITE);
    EndDrawing();
  }
}
