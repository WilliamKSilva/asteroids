#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "timer.h"
#include <memory>

using namespace std;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PLAYER_MAX_SPEED 10.0
#define PLAYER_IMPULSE_SPEED 0.6
#define PLAYER_DRAG_SPEED 0.2
#define ASTEROID_SPEED 7.0
#define ASTEROID_SPAWN_TIME 2.0
#define PROJECTILE_SPEED 20.0

class Circle
{
public:
  int centerX;
  int centerY;
  float radius;
  Color color;
};

class TexturePro
{
public:
  Rectangle sourceRec;
  Rectangle destRec;
  Vector2 origin;
  float rotation;
};

class GameObject
{
public:
  Vector2 position;
  Texture2D sprite;
  TexturePro texturePro;

  GameObject(Vector2 position, const char *spritePath)
  {
    this->position = position;
    this->sprite = LoadTexture(spritePath);

    TexturePro texturePro;
    texturePro.destRec = {
        .x = position.x,
        .y = position.y,
        .width = (float)this->sprite.width,
        .height = (float)this->sprite.height,
    };
    texturePro.origin = {
        .x = 0.0,
        .y = 0.0};
    texturePro.rotation = 0.0;
    texturePro.sourceRec = {
        .x = 0.0,
        .y = 0.0,
        .width = (float)this->sprite.width,
        .height = (float)this->sprite.height,
    };

    this->texturePro = texturePro;
  };

  void render()
  {
    DrawTexturePro(
        sprite,
        texturePro.sourceRec,
        texturePro.destRec,
        texturePro.origin,
        texturePro.rotation,
        WHITE);
  }

  void updatePosition(Vector2 position)
  {
    this->position = position;
    this->texturePro.destRec.x = position.x;
    this->texturePro.destRec.y = position.y;
  }

  bool isOutOfBounds()
  {
    return position.x > 1920 ||
           position.x < 0 ||
           position.y > 1080 ||
           position.y < 0;
  }
};

class Player : GameObject
{
public:
  float speed;

  Player(Vector2 position, const char *spritePath, float speed) : GameObject(position, spritePath)
  {
    this->speed = speed;
  }

  void move()
  {
    if (IsKeyDown(KEY_A))
    {
      texturePro.rotation -= 5;
    }

    if (IsKeyDown(KEY_D))
    {
      texturePro.rotation += 5;
    }

    if (IsKeyDown(KEY_W))
    {
      if (speed < PLAYER_MAX_SPEED)
      {
        speed += PLAYER_IMPULSE_SPEED;
      }

      Vector2 newPosition = position;
      newPosition.x += (sin(texturePro.rotation * DEG2RAD) * speed);
      newPosition.y -= (cos(texturePro.rotation * DEG2RAD) * speed);
      updatePosition(newPosition);
    }

    if (IsKeyUp(KEY_W))
    {
      if (speed > 0)
        speed -= PLAYER_DRAG_SPEED;

      if (speed < 0)
      {
        speed = 0.0;
        return;
      }

      Vector2 newPosition = position;
      newPosition.x += (sin(texturePro.rotation * DEG2RAD) * speed);
      newPosition.y -= (cos(texturePro.rotation * DEG2RAD) * speed);
      updatePosition(newPosition);
    }
  }
};

class Asteroid : GameObject
{
public:
  enum possibleSpawns {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
  };
  AsteroidSpawn spawn;
  int spawnLimit = RIGHT - TOP;

  Asteroid(Vector2 position, const char *spritePath) : GameObject(position, spritePath)
  {
    AsteroidSpawn spawn = (AsteroidSpawn)randomNumber(spawnLimit);

    if (spawn == TOP)
    {
      int randomX = randomNumber(1890);
      position.x = randomX;
      position.y = -5;
    }

    if (spawn == BOTTOM)
    {
      int randomX = randomNumber(1890);
      position.x = randomX;
      position.y = 1085;
    }

    if (LEFT)
    {
      int randomY = randomNumber(1050);
      position.y = randomY;
      position.x = -5;
    }

    if (RIGHT)
    {
      int randomY = randomNumber(1050);
      position.y = randomY;
      position.x = 1925;
    }

    this->spawn = spawn;
  };

  void move()
  {
    if (spawn == TOP)
    {
      Vector2 position = position;
      position.y += ASTEROID_SPEED;
      updatePosition(position);
      return;
    }

    if (spawn == BOTTOM)
    {
      Vector2 position = position;
      position.y -= ASTEROID_SPEED;
      updatePosition(position);
      return;
    }

    if (spawn == LEFT)
    {
      Vector2 position = position;
      position.x += ASTEROID_SPEED;
      updatePosition(position);
      return;
    }

    if (spawn == RIGHT)
    {
      Vector2 position = position;
      position.x -= ASTEROID_SPEED;
      updatePosition(position);
      return;
    }
  }
};

class Projectile : GameObject
{
public:
};

int randomNumber(int limit)
{
  srand(time(NULL));

  return (rand() % limit) + 1;
}

void UpdateAsteroids(Array **asteroids, Player player, bool *gameRunning)
{
  for (int i = 0; i < (*asteroids)->length; i++)
  {
    Asteroid *asteroid = (*asteroids)->data[i];
    if (asteroid == NULL)
      continue;

    MoveAsteroid(asteroid);
    bool asteroidAndPlayerCollision = CheckCollisionRecs(
        asteroid->gameObject->texturePro->destRec,
        player.gameObject->texturePro->destRec);

    if (asteroidAndPlayerCollision)
    {
      *asteroids = RemoveAsteroid(asteroids, i);
      // *gameRunning = false;
      break;
    }

    if (OutOfBoundsAsteroid(*asteroid))
    {
      *asteroids = RemoveAsteroid(asteroids, i);
    }
  }
}

Asteroid *CheckAsteroidsCollision(Array *asteroids, Rectangle objectToBeChecked)
{
  for (int i = 0; i < asteroids->length; ++i)
  {
    Asteroid *asteroid = asteroids->data[i];

    if (asteroid == NULL)
      continue;

    bool collisionWithAsteroid = CheckCollisionRecs(
        asteroid->gameObject->texturePro->destRec,
        objectToBeChecked);

    if (collisionWithAsteroid)
    {
      return asteroids->data[i];
    }
  }

  return NULL;
}

void RenderAsteroids(Array *asteroids)
{
  for (int i = 0; i < asteroids->length; ++i)
  {
    Asteroid *asteroid = (Asteroid *)asteroids->data[i];

    if (asteroid == NULL)
      continue;

    RenderGameObject(*asteroid->gameObject);
  }
}

void SpawnProjectile(Array *projectiles, Vector2 playerPosition, float playerRotation)
{
  if (IsKeyPressed(KEY_SPACE))
  {
    float xAxisPosition = playerPosition.x + (sin(playerRotation * DEG2RAD) * 70);
    float yAxisPosition = playerPosition.y - (cos(playerRotation * DEG2RAD) * 70);
    Vector2 position = {
        .x = xAxisPosition,
        .y = yAxisPosition};

    GameObject *gameObject = BuildGameObject(position, "./assets/projectile.png");
    gameObject->texturePro->rotation = playerRotation;
    Projectile *projectile = BuildProjectile(gameObject);

    if (projectiles->length == 0)
    {
      projectiles->length++;
      projectiles->data = malloc(sizeof(Asteroid *));
      projectiles->data[0] = projectile;
      return;
    }

    projectiles->length++;
    projectiles->data = realloc(projectiles->data, projectiles->length * sizeof(Asteroid *));
    projectiles->data[projectiles->length - 1] = projectile;
  }
}

void MoveProjectile(Projectile *projectile)
{
  Vector2 newPosition = projectile->gameObject->position;
  newPosition.x += sin(projectile->gameObject->texturePro->rotation * DEG2RAD) * PROJECTILE_SPEED;
  newPosition.y -= cos(projectile->gameObject->texturePro->rotation * DEG2RAD) * PROJECTILE_SPEED;
  UpdateGameObjectPosition(projectile->gameObject, newPosition);
}

void UpdateProjectiles(Array *projectiles, Array *asteroids, Player player)
{
  for (int i = 0; i < projectiles->length; ++i)
  {
    Projectile *projectile = projectiles->data[i];

    if (projectile == NULL)
      continue;

    MoveProjectile(projectile);

    // Check collision with each existing asteroid
    Asteroid *asteroidCollidedWith = CheckAsteroidsCollision(
        asteroids,
        projectile->gameObject->texturePro->destRec);

    if (asteroidCollidedWith != NULL)
    {
      free(projectile);
      projectiles->data[i] = NULL;
      projectiles->length--;

      free(asteroidCollidedWith);
      asteroidCollidedWith = NULL;
      asteroids->length--;
      break;
    }
    else
    {
      continue;
    }

    if (OutOfBoundsGameObject(projectile->gameObject->position))
    {
      free(projectile);
      projectiles->data[i] = NULL;
      continue;
    }
  }
}

void RenderProjectiles(Array *projectiles)
{
  for (int i = 0; i < projectiles->length; ++i)
  {
    Projectile *projectile = (Projectile *)projectiles->data[i];

    if (projectile == NULL)
      continue;

    RenderGameObject(*projectile->gameObject);
  }
}

void RestartGameState(Array *asteroids, Player *player, bool *gameRunning)
{
  // Reset asteroids
  for (int i = 0; i < asteroids->length; i++)
  {
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
      .y = 1080.0 / 2.0};
  UpdateGameObjectPosition(player->gameObject, position);

  *gameRunning = true;
}

int main()
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  SetTargetFPS(60);

  bool gameRunning = true;

  Vector2 playerPosition = {
      .x = (float)SCREEN_WIDTH / 2.0,
      .y = (float)SCREEN_HEIGHT / 2.0};

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
  asteroids->data = malloc(sizeof(Asteroid *));

  Array projectiles = {
      .length = 0,
      .data = NULL,
  };

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // TODO: Update should be framerate independent
    // Update
    //----------------------------------------------------------------------------------
    if (gameRunning)
    {
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
      if (TimerDone(asteroidSpawnTimer))
      {
        SpawnAsteroid(&asteroids);
        ResetTimer(&asteroidSpawnTimer);
        StartTimer(&asteroidSpawnTimer, ASTEROID_SPAWN_TIME);
        continue;
      }
      //----------------------------------------------------------------------------------
    }
    else
    {
      if (IsKeyPressed(KEY_ENTER))
        RestartGameState(asteroids, &player, &gameRunning);
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    if (gameRunning)
    {
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
    }
    else
    {
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
