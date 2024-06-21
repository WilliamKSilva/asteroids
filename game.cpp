#include <vector>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "timer.h"

using namespace std;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PLAYER_MAX_SPEED 10.0
#define PLAYER_IMPULSE_SPEED 0.6
#define PLAYER_DRAG_SPEED 0.2
#define ASTEROID_SPEED 7.0
#define ASTEROID_SPAWN_TIME 2.0
#define PROJECTILE_SPEED 20.0

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
  GameObject *gameObject;
} Projectile;

typedef struct {
  int index;
  Asteroid* element;
} Collision;

const int asteroidSpawnLimit = RIGHT - TOP; 

int RandomNumber(int limit) {
  srand(time(NULL));

  return (rand() % limit) + 1;
}

GameObject* BuildGameObject(Vector2 position, const char* spritePath) {
  GameObject *gameObject = new GameObject;
  Texture2D *sprite = new Texture2D;
  *sprite = LoadTexture(spritePath);
  gameObject->position = position;
  gameObject->sprite = sprite;

  TexturePro *texturePro = new TexturePro;
  *texturePro = (TexturePro){
    .sourceRec = {
      .x = 0,
      .y = 0,
      .width = (float)sprite->width,
      .height = (float)sprite->height 
    },
    .destRec = {
      .x = position.x,
      .y = position.y,
      .width = (float)sprite->width,
      .height = (float)sprite->height 
    },
    .origin = {
      .x = (float)sprite->width / 2,
      .y = (float)sprite->height / 2
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

  Asteroid *asteroid = new Asteroid;
  asteroid->gameObject = gameObject;
  asteroid->spawn = (AsteroidSpawn)randomSpawn;

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

void SpawnAsteroid(vector<Asteroid*> &asteroids) {
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
  asteroids.push_back(asteroid);
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
  Projectile *projectile = new Projectile; 
  projectile->gameObject = gameObject;

  return projectile;
}

void UpdateAsteroids(vector<Asteroid*> &asteroids, Player player, bool *gameRunning) {
  for (int i = 0; i < asteroids.size(); i++) {
    Asteroid *asteroid = asteroids[i];
    if (asteroid == NULL)
      continue;

    MoveAsteroid(asteroid);
    bool asteroidAndPlayerCollision = CheckCollisionRecs(
      asteroid->gameObject->texturePro->destRec,
      player.gameObject->texturePro->destRec
    );

    if (asteroidAndPlayerCollision) {
      delete asteroid;
      asteroids[i] = NULL;
      *gameRunning = false;
      break;
    }

    if (OutOfBoundsAsteroid(*asteroid)) {
      delete asteroid;
      asteroids[i] = NULL;
    }
  }
}

Collision CheckAsteroidsCollision(vector<Asteroid*> &asteroids, Rectangle objectToBeChecked) {
  for (int i = 0; i < asteroids.size(); ++i) {
    Asteroid *asteroid = asteroids[i]; 

    if (asteroid == NULL)
      continue;

    bool collisionWithAsteroid = CheckCollisionRecs(
      asteroid->gameObject->texturePro->destRec,
      objectToBeChecked
    );

    if (collisionWithAsteroid) {
      return {
        .index = i,
        .element = asteroid,
      };
    }
  }

  return {
    .index = 0,
    .element = NULL,
  };
}

void RenderAsteroids(vector<Asteroid*> &asteroids) {
  for (int i = 0; i < asteroids.size(); ++i) {
    Asteroid *asteroid = (Asteroid*)asteroids[i];

    if (asteroid == NULL)
      continue;

    RenderGameObject(*asteroid->gameObject);
  }
}

void SpawnProjectile(vector<Projectile*> &projectiles, Vector2 playerPosition, float playerRotation) {
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

    projectiles.push_back(projectile);
  }
}

void MoveProjectile(Projectile *projectile) {
  Vector2 newPosition = projectile->gameObject->position;
  newPosition.x += sin(projectile->gameObject->texturePro->rotation * DEG2RAD) * PROJECTILE_SPEED;
  newPosition.y -= cos(projectile->gameObject->texturePro->rotation * DEG2RAD) * PROJECTILE_SPEED;
  UpdateGameObjectPosition(projectile->gameObject, newPosition);
}

void UpdateProjectiles(vector<Projectile*> &projectiles, vector<Asteroid*> &asteroids, Player player) {
  for (int i = 0; i < projectiles.size(); ++i) {
    Projectile *projectile = projectiles[i];

    if (projectile == NULL)
      continue;

    MoveProjectile(projectile);

    // Check collision with each existing asteroid
    Collision collision = CheckAsteroidsCollision(
      asteroids,
      projectile->gameObject->texturePro->destRec
    );

    if (collision.element) {
      projectiles.erase(projectiles.begin() + i);
      delete projectile;
      projectiles[i] = NULL;

      asteroids.erase(asteroids.begin() + collision.index);
      delete collision.element;
      asteroids[collision.index] = NULL;
      break;
    } else {
      continue;
    }

    if (OutOfBoundsGameObject(projectile->gameObject->position)) {
      delete projectile;
      projectiles[i] = NULL;
      continue;
    }
  }
}

void RenderProjectiles(vector<Projectile*> &projectiles) {
  for (int i = 0; i < projectiles.size(); ++i) {
    Projectile *projectile = projectiles[i];

    if (projectile == NULL)
      continue;

    RenderGameObject(*projectile->gameObject);
  }
}

void RestartGameState(vector<Asteroid*> &asteroids, Player *player, bool *gameRunning) {
  // TODO: deallocate data and reset asteroids
  for (int i = 0; i < asteroids.size(); ++i) {
    delete asteroids[i];
    asteroids[i] = NULL;
  }

  asteroids.clear();

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
    .startTime = 0.0,
    .lifeTime = 0.0,
    .started = false,
  };
  StartTimer(&asteroidSpawnTimer, ASTEROID_SPAWN_TIME);

  vector<Asteroid*> asteroids;
  vector<Projectile*> projectiles;

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // TODO: Update should be framerate independent
    // Update
    //----------------------------------------------------------------------------------
    if (gameRunning) {
      // Input
      MovePlayer(&player);
      SpawnProjectile(projectiles, player.gameObject->position, player.gameObject->texturePro->rotation);
      //----------------------------------------------------------------------------------

      // Asteroids Update 
      UpdateAsteroids(asteroids, player, &gameRunning); 
      //----------------------------------------------------------------------------------
      
      // Projectiles Update 
      UpdateProjectiles(projectiles, asteroids, player); 
      //----------------------------------------------------------------------------------

      // Timers based logic 
      if (TimerDone(asteroidSpawnTimer)) {
        SpawnAsteroid(asteroids); 
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
          RenderProjectiles(projectiles); 
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
