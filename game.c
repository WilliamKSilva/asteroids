#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "timer.h"
#include "array.h"
#include "game.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PLAYER_MAX_SPEED 10.0
#define PLAYER_IMPULSE_SPEED 0.6
#define PLAYER_DRAG_SPEED 0.2
#define ASTEROID_SPEED 7.0
#define ASTEROID_SPAWN_TIME 2.0
#define PROJECTILE_SPEED 20.0

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

bool OutOfBoundsGameObject(Vector2 position) {
  return 
    position.x > 1920 ||
    position.x < 0 ||
    position.y > 1080 ||
    position.y < 0;
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

void SpawnAsteroid(Array **asteroids) {
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

  *asteroids = PushAsteroid(asteroids, asteroid);
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

Projectile* BuildProjectile(GameObject *gameObject) {
  Projectile *projectile = malloc(sizeof(Projectile)); 
  projectile->gameObject = gameObject;

  return projectile;
}

void UpdateAsteroids(Array **asteroids, Player player, bool *gameRunning) {
  for (int i = 0; i < (*asteroids)->length; i++) {
    Asteroid *asteroid = (*asteroids)->data[i];
    if (asteroid == NULL)
      continue;

    MoveAsteroid(asteroid);
    bool asteroidAndPlayerCollision = CheckCollisionRecs(
      asteroid->gameObject->texturePro->destRec,
      player.gameObject->texturePro->destRec
    );

    if (asteroidAndPlayerCollision) {
      *asteroids = RemoveAsteroid(asteroids, i);
      // *gameRunning = false;
      break;
    }

    if (OutOfBoundsAsteroid(*asteroid)) {
      *asteroids = RemoveAsteroid(asteroids, i);
    }
  }
}

Asteroid* CheckAsteroidsCollision(Array *asteroids, Rectangle objectToBeChecked) {
  for (int i = 0; i < asteroids->length; ++i) {
    Asteroid *asteroid = asteroids->data[i]; 

    if (asteroid == NULL)
      continue;

    bool collisionWithAsteroid = CheckCollisionRecs(
      asteroid->gameObject->texturePro->destRec,
      objectToBeChecked
    );

    if (collisionWithAsteroid) {
      return asteroids->data[i];
    }
  }

  return NULL;
}

void RenderAsteroids(Array *asteroids) {
  for (int i = 0; i < asteroids->length; ++i) {
    Asteroid *asteroid = (Asteroid*)asteroids->data[i];

    if (asteroid == NULL)
      continue;

    RenderGameObject(*asteroid->gameObject);
  }
}

void SpawnProjectile(Array *projectiles, Vector2 playerPosition, float playerRotation) {
  if (IsKeyPressed(KEY_SPACE)) {
    float xAxisPosition = playerPosition.x + (sin(playerRotation * DEG2RAD) * 70); 
    float yAxisPosition = playerPosition.y - (cos(playerRotation * DEG2RAD) * 70); 
    Vector2 position = {
      .x = xAxisPosition,
      .y = yAxisPosition
    };

    GameObject *gameObject = BuildGameObject(position, "./assets/projectile.png"); 
    gameObject->texturePro->rotation = playerRotation;
    Projectile *projectile = BuildProjectile(gameObject);

    if (projectiles->length == 0) {
      projectiles->length++;
      projectiles->data = malloc(sizeof(Asteroid*));
      projectiles->data[0] = projectile;
      return;
    }

    projectiles->length++;
    projectiles->data = realloc(projectiles->data, projectiles->length * sizeof(Asteroid*));
    projectiles->data[projectiles->length - 1] = projectile;
  }
}

void MoveProjectile(Projectile *projectile) {
  Vector2 newPosition = projectile->gameObject->position;
  newPosition.x += sin(projectile->gameObject->texturePro->rotation * DEG2RAD) * PROJECTILE_SPEED;
  newPosition.y -= cos(projectile->gameObject->texturePro->rotation * DEG2RAD) * PROJECTILE_SPEED;
  UpdateGameObjectPosition(projectile->gameObject, newPosition);
}

void UpdateProjectiles(Array *projectiles, Array* asteroids, Player player) {
  for (int i = 0; i < projectiles->length; ++i) {
    Projectile *projectile = projectiles->data[i];

    if (projectile == NULL)
      continue;

    MoveProjectile(projectile);

    // Check collision with each existing asteroid
    Asteroid *asteroidCollidedWith = CheckAsteroidsCollision(
      asteroids,
      projectile->gameObject->texturePro->destRec
    );

    if (asteroidCollidedWith != NULL) {
      free(projectile);
      projectiles->data[i] = NULL;
      projectiles->length--;

      free(asteroidCollidedWith);
      asteroidCollidedWith = NULL;
      asteroids->length--;
      break;
    } else {
      continue;
    }

    if (OutOfBoundsGameObject(projectile->gameObject->position)) {
      free(projectile);
      projectiles->data[i] = NULL;
      continue;
    }
  }
}

void RenderProjectiles(Array *projectiles) {
  for (int i = 0; i < projectiles->length; ++i) {
    Projectile *projectile = (Projectile*)projectiles->data[i];

    if (projectile == NULL)
      continue;

    RenderGameObject(*projectile->gameObject);
  }
}

void RestartGameState(Array *asteroids, Player *player, bool *gameRunning) {
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

  Array *asteroids = malloc(sizeof(Array));
  asteroids->length = 0;
  asteroids->data = malloc(sizeof(Asteroid*));

  Array projectiles = {
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
      SpawnProjectile(&projectiles, player.gameObject->position, player.gameObject->texturePro->rotation);
      //----------------------------------------------------------------------------------

      // Asteroids Update 
      UpdateAsteroids(&asteroids, player, &gameRunning); 
      //----------------------------------------------------------------------------------
      
      // Projectiles Update 
      UpdateProjectiles(&projectiles, asteroids, player); 
      //----------------------------------------------------------------------------------

      // Timers based logic 
      if (TimerDone(asteroidSpawnTimer)) {
        SpawnAsteroid(&asteroids); 
        ResetTimer(&asteroidSpawnTimer);
        StartTimer(&asteroidSpawnTimer, ASTEROID_SPAWN_TIME);
        continue;
      }
      //----------------------------------------------------------------------------------
    } else {
      if (IsKeyPressed(KEY_ENTER))
        RestartGameState(asteroids, &player, &gameRunning);
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
      BeginDrawing();
        if (gameRunning) {
          ClearBackground(BLACK);

          // Player Render
          RenderGameObject(*player.gameObject);
          //----------------------------------------------------------------------------------

          // Asteroid Render
          RenderAsteroids(asteroids); 
          //----------------------------------------------------------------------------------
      
          // Projectile Render
          RenderProjectiles(&projectiles); 
          //----------------------------------------------------------------------------------
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
