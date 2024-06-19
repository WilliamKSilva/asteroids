#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "timer.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PLAYER_MAX_SPEED 10.0
#define PLAYER_IMPULSE_SPEED 0.6
#define PLAYER_DRAG_SPEED 0.2
#define ASTEROID_SPEED 7.0
#define ASTEROID_SPAWN_TIME 2.0

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
  GameObject *gameObject;
  float speed;
} Player;

typedef enum {
  TOP,
  BOTTOM,
  LEFT,
  RIGHT
} AsteroidSpawn;

typedef struct {
  GameObject *gameObject;
  AsteroidSpawn spawn;
} Asteroid;

typedef struct {
  int length;
  void** data;
} List;

const int asteroidSpawnLimit = RIGHT - TOP; 

int RandomNumber(int limit) {
  srand(time(NULL));

  return (rand() % limit) + 1;
}

GameObject* BuildGameObject(Vector2 position, const char* spritePath) {
  GameObject *gameObject = malloc(sizeof(GameObject));
  Texture2D *sprite = malloc(sizeof(Texture2D));
  *sprite = LoadTexture(spritePath);
  gameObject->position = position;
  gameObject->sprite = sprite;

  TexturePro *texturePro = malloc(sizeof(TexturePro));
  *texturePro = (TexturePro){
    .sourceRec = {
      .x = 0,
      .y = 0,
      .width = sprite->width,
      .height = sprite->height 
    },
    .destRec = {
      .x = position.x,
      .y = position.y,
      .width = sprite->width,
      .height = sprite->height 
    },
    .origin = {
      .x = sprite->width / 2.0,
      .y = sprite->height / 2.0
    },
    .rotation = 0
  };
  
  gameObject->texturePro = texturePro;

  return gameObject;
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

Player BuildPlayer(GameObject *gameObject) {
  Player player = {
    .gameObject = gameObject,
    .speed = 0.0
  };

  return player;
}

void MovePlayer(Player *player) {
  if (player->gameObject->texturePro == NULL) {
    printf("%s\n", "Missing texture pro attributes");
    return;
  }

  if (IsKeyDown(KEY_A)) {
    player->gameObject->texturePro->rotation -= 5;
  }

  if (IsKeyDown(KEY_D)) {
    player->gameObject->texturePro->rotation += 5;
  }

  if (IsKeyDown(KEY_W)) {
    if (player->speed < PLAYER_MAX_SPEED) {
      player->speed += PLAYER_IMPULSE_SPEED;
    }

    Vector2 newPosition = player->gameObject->position;
    newPosition.x += (sin(player->gameObject->texturePro->rotation * DEG2RAD) * player->speed);
    newPosition.y -= (cos(player->gameObject->texturePro->rotation * DEG2RAD) * player->speed);
    UpdateGameObjectPosition(player->gameObject, newPosition);
  }

  if (IsKeyUp(KEY_W)) {
    if (player->speed > 0)
      player->speed -= PLAYER_DRAG_SPEED;

    if (player->speed < 0) {
      player->speed = 0.0;
      return;
    }

    Vector2 newPosition = player->gameObject->position;
    newPosition.x += (sin(player->gameObject->texturePro->rotation * DEG2RAD) * player->speed);
    newPosition.y -= (cos(player->gameObject->texturePro->rotation * DEG2RAD) * player->speed);
    UpdateGameObjectPosition(player->gameObject, newPosition);
  }
}

Asteroid* BuildAsteroid(GameObject *gameObject) {
  int randomSpawn = RandomNumber(asteroidSpawnLimit);

  Asteroid *asteroid = malloc(sizeof(Asteroid));
  asteroid->gameObject = gameObject;
  asteroid->spawn = randomSpawn;

  return asteroid;
}

void MoveAsteroid(Asteroid *asteroid) {
  if (asteroid->spawn == TOP) {
    Vector2 position = asteroid->gameObject->position;
    position.y += ASTEROID_SPEED;
    UpdateGameObjectPosition(asteroid->gameObject, position);
    return;
  }

  if (asteroid->spawn == BOTTOM) {
    Vector2 position = asteroid->gameObject->position;
    position.y -= ASTEROID_SPEED;
    UpdateGameObjectPosition(asteroid->gameObject, position);
    return;
  }

  if (asteroid->spawn == LEFT) {
    Vector2 position = asteroid->gameObject->position;
    position.x += ASTEROID_SPEED;
    UpdateGameObjectPosition(asteroid->gameObject, position);
    return;
  }

  if (asteroid->spawn == RIGHT) {
    Vector2 position = asteroid->gameObject->position;
    position.x -= ASTEROID_SPEED;
    UpdateGameObjectPosition(asteroid->gameObject, position);
    return;
  }
}

void SpawnAsteroid(List *asteroids) {
  Vector2 position;
  GameObject *gameObject = BuildGameObject(position, "./assets/asteroid.png");
  Asteroid *asteroid = BuildAsteroid(gameObject);
  if (asteroid->spawn == TOP) {
    int randomX = RandomNumber(1890);
    position.x = randomX;
    position.y = -5;
  }

  if (asteroid->spawn == BOTTOM) {
    int randomX = RandomNumber(1890);
    position.x = randomX;
    position.y = 1085;
  }

  if (asteroid->spawn == LEFT) {
    int randomY = RandomNumber(1050);
    position.y = randomY;
    position.x = -5;
  }

  if (asteroid->spawn == RIGHT) {
    int randomY = RandomNumber(1050);
    position.y = randomY;
    position.x = 1925;
  }

  UpdateGameObjectPosition(asteroid->gameObject, position);

  if (asteroids->length == 0) {
    asteroids->data = malloc(sizeof(Asteroid*));
    asteroids->data[0] = asteroid;
    asteroids->length++;
  } else {
    asteroids->length++;
    asteroids->data = realloc(asteroids->data, sizeof(Asteroid*) * asteroids->length);
    asteroids->data[asteroids->length - 1] = asteroid;
  }
}

bool OutOfBoundsAsteroid(Asteroid asteroid) {
  Vector2 position = asteroid.gameObject->position;
  if (asteroid.spawn == TOP)
    return position.y > SCREEN_HEIGHT;

  if (asteroid.spawn == BOTTOM)
    return position.y < 0;

  if (asteroid.spawn == LEFT)
    return position.x > SCREEN_WIDTH;

  if (asteroid.spawn == LEFT)
    return position.x < 0;

  return false;
}

void RestartGameState(List *asteroids, Player *player, bool *gameRunning) {
  // Reset asteroids
  for (int i = 0; i < asteroids->length; i++) {
    if (asteroids->data[i] == NULL)
      continue;

    free(asteroids->data[i]);
    asteroids->data[i] = NULL;
  }

  free(asteroids->data);
  asteroids->data = NULL;
  asteroids->length = 0;

  // Reset player
  Vector2 position = {
    .x = 1920.0 / 2.0,
    .y = 1080.0 / 2.0
  };
  UpdateGameObjectPosition(player->gameObject, position);

  *gameRunning = true;
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  bool gameRunning = true;

  Vector2 playerPosition = {
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };

  GameObject *gameObject = BuildGameObject(playerPosition, "./assets/player.png");
  Player player = BuildPlayer(gameObject); 

  Timer asteroidSpawnTimer = {
    .lifeTime = 0.0,
    .startTime = 0.0,
    .started = false,
  };
  StartTimer(&asteroidSpawnTimer, ASTEROID_SPAWN_TIME);

  List asteroidsList = {
    .length = 0,
    .data = NULL,
  };

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // TODO: Update should be framerate independent
    // Update
    //----------------------------------------------------------------------------------
    if (gameRunning) {
      // Input
      MovePlayer(&player);

      // Spawn
      if (TimerDone(asteroidSpawnTimer)) {
        SpawnAsteroid(&asteroidsList); 
        ResetTimer(&asteroidSpawnTimer);
        StartTimer(&asteroidSpawnTimer, ASTEROID_SPAWN_TIME);
        continue;
      }

      // Asteroids Logic 
      for (int i = 0; i < asteroidsList.length; i++) {
        Asteroid *asteroid = asteroidsList.data[i];
        if (asteroid == NULL)
          continue;

        MoveAsteroid(asteroid);
        bool asteroidAndPlayerCollision = CheckCollisionRecs(
          asteroid->gameObject->texturePro->destRec,
          player.gameObject->texturePro->destRec
        );

        if (asteroidAndPlayerCollision) {
          free(asteroid);
          asteroidsList.data[i] = NULL;
          gameRunning = false;
          break;
        }

        if (OutOfBoundsAsteroid(*asteroid)) {
          free(asteroid);
          asteroidsList.data[i] = NULL;
        }
      }
    } else {
      if (IsKeyPressed(KEY_ENTER))
        RestartGameState(&asteroidsList, &player, &gameRunning);
    }

    // Draw
    //----------------------------------------------------------------------------------
      BeginDrawing();
        if (gameRunning) {
          ClearBackground(BLACK);
          RenderGameObject(*player.gameObject);
          for (int j = 0; j < asteroidsList.length; j++) {
            Asteroid *asteroid = (Asteroid*)asteroidsList.data[j];

            if (asteroid == NULL)
              continue;

            RenderGameObject(*asteroid->gameObject);
          }
        } else {
          DrawText("Game Over - Press Enter to restart", 1920 / 2 - 320, 1080 / 2 - 100, 31, RED);
        }
      EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------
  return 0;
}
