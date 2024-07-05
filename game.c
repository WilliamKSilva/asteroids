#include "stdlib.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "timer.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

#define PLAYER_ROTATION_SPEED 4.0
#define PLAYER_IMPULSE 1.0
#define PLAYER_MAX_IMPULSE 10.0
#define PLAYER_DRAG 0.5

#define PROJECTILE_SPEED 15.0
#define PROJECTILE_START_POSITION_SCALE 70 

#define ASTEROID_SPEED 5.0

#define ENEMY_SPEED 3.0

#define BIG_ASTEROID_SCORE 20

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
  Sound explode;
  Sound shoot;
  Sound thrust;
  Sound asteroidDestroyed;
} Sounds;

typedef struct {
  void *ptr;
  int length;
  size_t itemSize;
} Array;

typedef struct {
  TexturePro texture;
  float speed;
  int lifes;
  int score;
} Player;

typedef struct {
  TexturePro texture;
} Projectile;

typedef enum {
  TOP,
  BOTTOM,
  LEFT,
  RIGHT
} Spawn;

typedef struct {
  TexturePro texture;
  Spawn spawn; 
  bool diagonalMove; 
} Asteroid;

typedef struct {
  TexturePro texture;
  Spawn spawn;
} Enemy;

typedef struct {
  Vector2 position;
  float rotation; // Object facing torwards rotation based on spawn
} StartPositionBasedOnSpawn;

const int spawnLimit = RIGHT; 

int randomNumber(int limit) {
  return rand() % limit;
}

StartPositionBasedOnSpawn getStartPositionBasedOnSpawn(Spawn spawn, bool getRotation) {
  StartPositionBasedOnSpawn startPosition;
  if (spawn == RIGHT) {
    startPosition.position.x = SCREEN_WIDTH + 5;
    startPosition.position.y = randomNumber(SCREEN_HEIGHT);

    if (getRotation)
      startPosition.rotation = 270.0;

    return startPosition;
  }

  if (spawn == LEFT) {
    startPosition.position.x = -5;
    startPosition.position.y = randomNumber(SCREEN_HEIGHT);

    if (getRotation)
      startPosition.rotation = 90.0;

    return startPosition;
  }

  if (spawn == TOP) {
    startPosition.position.x = randomNumber(SCREEN_WIDTH);
    startPosition.position.y = -5;

    if (getRotation)
      startPosition.rotation = 180.0;

    return startPosition;
  }

  if (spawn == BOTTOM) {
    startPosition.position.x = randomNumber(SCREEN_WIDTH);
    startPosition.position.y = SCREEN_HEIGHT + 5;

    if (getRotation)
      startPosition.rotation = 0.0;

    return startPosition;
  }

  return startPosition;
}

Vector2 moveObjectBasedOnSpawn(Vector2 currentPosition, Spawn spawn, float speed, bool diagonalMove) {
  Vector2 position = currentPosition;
  if (spawn == TOP) {
    position.y += speed;

    if (diagonalMove)
      position.x += speed;

    return position;
  }

  if (spawn == BOTTOM) {
    position.y -= speed;

    if (diagonalMove)
      position.x += speed;

    return position;
  }

  if (spawn == LEFT) {
    position.x += speed;

    if (diagonalMove)
      position.y += speed;

    return position;
  }

  if (spawn == RIGHT) {
    position.x -= speed;

    if (diagonalMove)
      position.y += speed;

    return position;
  }

  return position;
}

TexturePro buildTexturePro(Vector2 *startPosition, const char *spritePath) {
  Vector2 position;

  if (startPosition == NULL) {
    position.x = 0.0;
    position.y = 0.0;
  } else {
    position.x = startPosition->x;
    position.y = startPosition->y;
  }

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
      .x = position.x,
      .y = position.y,
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

void movePlayer(Player *player, Sound thrustSound) {
  if (IsKeyDown(KEY_A))
    player->texture.rotation -= PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_D))
    player->texture.rotation += PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_W)) {
    if (!IsSoundPlaying(thrustSound))
      PlaySound(thrustSound);

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

void spawnProjectile(Array *projectiles, Vector2 *startPosition, float rotation) {
  Projectile projectile = { 
    .texture = buildTexturePro(startPosition, "./assets/projectile.png")
  };

  projectile.texture.rotation = rotation;

  if (projectiles->length == 0) {
    Projectile* ptr = malloc(sizeof(Projectile));

    if (ptr == NULL) {
      printf("Error trying to allocate memory for projectiles\n");
      exit(-1);
    }

    ptr[0] = projectile;
    projectiles->ptr = ptr;
    projectiles->length++;
  } else {
    projectiles->length++;
    Projectile* ptr = realloc(projectiles->ptr, projectiles->length * sizeof(Projectile));

    if (ptr == NULL) {
      printf("Error trying to allocate memory for projectiles\n");
      exit(-1);
    }

    ptr[projectiles->length - 1] = projectile;

    projectiles->ptr = ptr;
  }

}

Vector2 getProjectileStartPosition(Player player) {
  Vector2 position = {
    .x = player.texture.dest.x,
    .y = player.texture.dest.y,
  };
  position.x += sin(player.texture.rotation * DEG2RAD) * PROJECTILE_START_POSITION_SCALE;
  position.y -= cos(player.texture.rotation * DEG2RAD) * PROJECTILE_START_POSITION_SCALE;

  return position;
}

void shootProjectile(Array *projectiles, Player player, Sound shootSound) {
  Vector2 projectilePosition = getProjectileStartPosition(player);
  if (IsKeyPressed(KEY_SPACE)) {
    spawnProjectile(projectiles, &projectilePosition, player.texture.rotation);
    PlaySound(shootSound);
  }
}

void moveProjectile(Projectile *projectile) {
  projectile->texture.dest.x += sin(projectile->texture.rotation * DEG2RAD) * PROJECTILE_SPEED;
  projectile->texture.dest.y -= cos(projectile->texture.rotation * DEG2RAD) * PROJECTILE_SPEED;
}

bool isObjectOutOfBounds(Vector2 position) {
  if (position.x > SCREEN_WIDTH + 5 || position.x < -5)
      return true;

  if (position.y > SCREEN_HEIGHT + 5 || position.y < -5)
      return true;

  return false;
}

bool checkObjectsCollision(Rectangle rec1, Rectangle rec2) {
  return CheckCollisionRecs(rec1, rec2);
} 

void deleteElementFromArray(Array *array, int indexToDelete) {
  if (indexToDelete < 0 || indexToDelete >= array->length)
    return;

  array->length--;

  char *data = (char*)array->ptr;  

  char *dest = data + indexToDelete * array->itemSize;
  char *source = data + (indexToDelete + 1) * array->itemSize;
  size_t offset = (array->length - indexToDelete) * array->itemSize;

  memmove(dest, source, offset);

  printf("INFO: object deleted\n");
}

void spawnAsteroid(Array *asteroids) {
  Asteroid asteroid = {
    .texture = buildTexturePro(NULL, "./assets/asteroid_big.png")
  };

  asteroid.spawn = randomNumber(spawnLimit + 1);

  // 0 or 1 = false or true
  asteroid.diagonalMove = randomNumber(2);

  StartPositionBasedOnSpawn startPosition = getStartPositionBasedOnSpawn(asteroid.spawn, false);

  asteroid.texture.dest.x = startPosition.position.x;
  asteroid.texture.dest.y = startPosition.position.y;

  if (asteroids->length == 0) {
    Asteroid *ptr = (Asteroid*)malloc(sizeof(Asteroid));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for asteroids\n");
      exit(-1);
    }

    ptr[0] = asteroid;
    asteroids->ptr = ptr;
    asteroids->length++;
  } else {
    asteroids->length++;
    Asteroid *ptr = (Asteroid*)realloc(asteroids->ptr, asteroids->length * sizeof(Asteroid));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for asteroids\n");
      exit(-1);
    }

    ptr[asteroids->length - 1] = asteroid;
    asteroids->ptr = ptr;
  }
}

void spawnEnemy(Array *enemies) {
  TexturePro texture = buildTexturePro(NULL, "./assets/enemy.png");

  Enemy enemy = {
    .texture = texture,
  };

  enemy.spawn = randomNumber(spawnLimit);

  StartPositionBasedOnSpawn startPosition = getStartPositionBasedOnSpawn(enemy.spawn, true);
  enemy.texture.dest.x = startPosition.position.x;
  enemy.texture.dest.y = startPosition.position.y;

  // Set enemy facing torwards direction based on spawn
  enemy.texture.rotation = startPosition.rotation;

  if (enemies->length == 0) {
    Enemy *ptr = (Enemy*)malloc(sizeof(Enemy));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for asteroids\n");
      exit(-1);
    }

    ptr[0] = enemy;
    enemies->ptr = ptr;
    enemies->length++;
  } else {
    enemies->length++;
    Enemy *ptr = (Enemy*)realloc(enemies->ptr, enemies->length * sizeof(Enemy));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for enemies\n");
      exit(-1);
    }

    ptr[enemies->length - 1] = enemy;
    enemies->ptr = ptr;
  }
}

void resetGameState(Player *player, Array *projectiles, Array *asteroids)
{
  // Reset player attributes 
  player->texture.dest.x = SCREEN_WIDTH / 2.0;
  player->texture.dest.y = SCREEN_HEIGHT / 2.0;
  player->score = 0;
  player->lifes = 2;

  free(projectiles->ptr);
  projectiles->ptr = NULL;
  projectiles->length = 0;

  free(asteroids->ptr);
  asteroids->ptr = NULL;
  asteroids->length = 0;
}

void update(
  Player *player,
  Array *projectiles,
  Array *asteroids,
  Array *enemies,
  Timer *asteroidSpawnTimer,
  Timer *enemySpawnTimer,
  Sounds sounds,
  bool *isGameRunning)
{
  // Input updates
  movePlayer(player, sounds.thrust);
  shootProjectile(projectiles, *player, sounds.shoot);

  // Scripted updates
  // Projectiles
  for (int i = 0; i < projectiles->length; ++i) {
    Projectile* ptr = projectiles->ptr;
    moveProjectile(&ptr[i]); 

    Projectile projectile = ptr[i];
    Vector2 projectilePosition = {
      .x = projectile.texture.dest.x,
      .y = projectile.texture.dest.y
    };

    if (isObjectOutOfBounds(projectilePosition)) {
      deleteElementFromArray(projectiles, i);
    }
  }

  // Asteroids
  for (int i = 0; i < asteroids->length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)asteroids->ptr;
    Asteroid asteroid = asteroidsData[i];

    Vector2 currentPosition = {
      .x = asteroid.texture.dest.x,
      .y = asteroid.texture.dest.y
    };

    Vector2 position = moveObjectBasedOnSpawn(currentPosition, asteroid.spawn, ASTEROID_SPEED, asteroid.diagonalMove);
    asteroidsData[i].texture.dest.x = position.x;
    asteroidsData[i].texture.dest.y = position.y;

    if (isObjectOutOfBounds(position))
    {
      deleteElementFromArray(asteroids, i);
      continue;
    }

    if (checkObjectsCollision(asteroidsData[i].texture.dest, player->texture.dest)) {
      if (player->lifes == 1) {
        PlaySound(sounds.explode);
        printf("GAME: game over\n");
        *isGameRunning = false;
        resetGameState(player, projectiles, asteroids);
        return;
      }

      player->lifes--;
      player->texture.dest.x = SCREEN_WIDTH / 2.0;
      player->texture.dest.y = SCREEN_HEIGHT / 2.0;
      deleteElementFromArray(asteroids, i);
      PlaySound(sounds.explode);
    }

    for (int j = 0; j < projectiles->length; ++j) {
      Projectile *projectileData = projectiles->ptr;

      if (checkObjectsCollision(projectileData[j].texture.dest, asteroidsData[i].texture.dest)) {
        deleteElementFromArray(asteroids, i);
        deleteElementFromArray(projectiles, j);

        player->score += BIG_ASTEROID_SCORE;
        PlaySound(sounds.asteroidDestroyed);
        break;
      }
    }
  }

  // Enemies
  for (int i = 0; i < enemies->length; ++i) {
    Enemy *enemiesData = (Enemy*)enemies->ptr;
    Enemy enemy = enemiesData[i];
    Vector2 currentPosition = {
      .x = enemy.texture.dest.x,
      .y = enemy.texture.dest.y,
    };

    Vector2 position = moveObjectBasedOnSpawn(
      currentPosition,
      enemy.spawn,
      ENEMY_SPEED,
      false
    );
    // TODO: test using "enemy" variable here
    enemiesData[i].texture.dest.x = position.x;
    enemiesData[i].texture.dest.y = position.y;

    if (isObjectOutOfBounds(position))
      deleteElementFromArray(enemies, i);
  }

  if (isTimerDone(asteroidSpawnTimer)) {
    // Spawn new Asteroid 
    spawnAsteroid(asteroids);
    
    // Restart timer
    startTimer(asteroidSpawnTimer, asteroidSpawnTimer->lifeTime);
  }

  if (isTimerDone(enemySpawnTimer)) {
    // Spawn new Enemy 
    spawnEnemy(enemies);
    
    // Restart timer
    startTimer(enemySpawnTimer, enemySpawnTimer->lifeTime);
  }
}

void render(
  Player player,
  Array projectiles,
  Array asteroids,
  Array enemies)
{
  renderTexturePro(player.texture);
  char lifeBuf[20];
  char scoreBuf[20];

  snprintf(scoreBuf, 20, "Score: %d", player.score);
  DrawText(scoreBuf, 50, 50, 25, WHITE);

  snprintf(lifeBuf, 20, "Lifes: %d", player.lifes);
  DrawText(lifeBuf, 50, 80, 25, RED);

  for (int i = 0; i < projectiles.length; ++i) {
    Projectile* ptr = (Projectile*)projectiles.ptr;
    renderTexturePro(ptr[i].texture); 
  }

  for (int i = 0; i < asteroids.length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)asteroids.ptr;
    renderTexturePro(asteroidsData[i].texture); 
  }

  for (int i = 0; i < enemies.length; ++i) {
    Enemy* enemiesData = (Enemy*)enemies.ptr;
    renderTexturePro(enemiesData[i].texture); 
  }
}

void renderGameOver() {
  DrawText(
    "Game Over - Press space to restart",
    SCREEN_WIDTH / 2,
    SCREEN_HEIGHT / 2,
    31,
    WHITE
  );
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  InitAudioDevice();
  SetTargetFPS(60);
  srand(time(NULL));

  bool isGameRunning = true;

  Sounds sounds = {
    .shoot = LoadSound("./assets/shoot.wav"),
    .explode = LoadSound("./assets/explode.wav"),
    .thrust = LoadSound("./assets/thrust.wav"),
    .asteroidDestroyed = LoadSound("./assets/asteroid.wav")
  };

  Vector2 playerStartPosition = { 
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  Player player = {
    .texture = buildTexturePro(&playerStartPosition, "./assets/player.png"),
    .speed = 0.0,
    .lifes = 2,
    .score = 0,
  };

  Timer asteroidSpawnTimer;
  startTimer(&asteroidSpawnTimer, 2.0);

  Timer enemySpawnTimer;
  startTimer(&enemySpawnTimer, 2.0);

  Array projectiles = {
    .ptr = NULL,
    .length = 0,
    .itemSize = sizeof(Projectile)
  };

  Array asteroids = {
    .ptr = NULL,
    .length = 0,
    .itemSize = sizeof(Asteroid)
  };

  Array enemies = {
    .ptr = NULL,
    .length = 0,
    .itemSize = sizeof(Enemy)
  };

  while (!WindowShouldClose()) {
    // Update logic 
    if (isGameRunning) {
      update(
        &player,
        &projectiles,
        &asteroids,
        &enemies,
        &asteroidSpawnTimer,
        &enemySpawnTimer,
        sounds,
        &isGameRunning
      );
    } else {
      if (IsKeyPressed(KEY_SPACE)) {
        isGameRunning = true;
      }
    }

    // Render logic
    BeginDrawing();
      ClearBackground(BLACK);
      if (isGameRunning) {
        render(player, projectiles, asteroids, enemies);
      } else {
        renderGameOver();
      }
    EndDrawing();
  }

  return 0;
}
