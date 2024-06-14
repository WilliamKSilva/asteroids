#include <stdio.h>
#include "raylib.h"

int main() {
  printf("%s", "teste\n");
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Asteroids");
  SetTargetFPS(30);
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
      // Update
      //----------------------------------------------------------------------------------
      // TODO: Update your variables here
      //----------------------------------------------------------------------------------

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();

          ClearBackground(BLACK);

          DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

      EndDrawing();
      //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow();        // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
  return 0;
}
