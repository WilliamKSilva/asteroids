#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "timer.h"
#include <unistd.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080 

#define PLAYER_ROTATION_SPEED 4.0
#define PLAYER_IMPULSE 1.0
#define PLAYER_MAX_IMPULSE 10.0
#define PLAYER_DRAG 0.5

#define PROJECTILE_PLAYER_SPEED 15.0
#define PROJECTILE_ENEMY_SPEED 10.0
#define PROJECTILE_START_POSITION_SCALE 70 

#define PLAYER_FIRE_EFFECT_POSITION_SCALE -55

#define ASTEROID_SPEED 5.0

#define ENEMY_SPEED 3.0

#define BIG_ASTEROID_SCORE 20
#define ENEMY_SCORE 10

// TODO: finish player damage effect rendering
// TODO: add proper score system
// TODO: add menu
// TODO: add smaller asteroids spawn
// TODO: add game state struct

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
  Texture2D playerShipFire;
  Texture2D damage;
} StaticAssets;

typedef enum {
  GAME_OVER,
  PLAYER_DEATH_PAUSE,
  RUNNING,
} GameStatus;

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
  bool isBoosting;
} Player;

typedef struct {
  TexturePro texture;
  bool enemyProjectile;
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
  Timer shootTimer; 
} Enemy;

typedef struct {
  Vector2 position;
  float rotation; // Object facing torwards rotation based on spawn
} StartPositionBasedOnSpawn;

typedef enum {
  ASTEROID,
  PROJECTILE,
  ENEMY
} CollidedObject;

const int spawnLimit = RIGHT; 


// TODO: this needs to be global but needs to be restructured
// to be part of an global state data structure
Timer playerDeathTimer;
Vector2 playerDeathPosition = {
  .x = 0.0,
  .y = 0.0
};

int randomNumber(int limit) {
  return rand() % limit;
}

StartPositionBasedOnSpawn getStartPositionBasedOnSpawn(Spawn spawn, bool getRotation) {
  StartPositionBasedOnSpawn startPosition = {
    .position = {
      .x = 0.0,
      .y = 0.0
    },
    .rotation = 0.0
  };

  if (spawn == RIGHT) {
    startPosition.position.x = (float)SCREEN_WIDTH + 5.0;
    startPosition.position.y = (float)randomNumber(SCREEN_HEIGHT);

    if (getRotation)
      startPosition.rotation = 270.0;

    return startPosition;
  }

  if (spawn == LEFT) {
    startPosition.position.x = -5.0;
    startPosition.position.y = (float)randomNumber(SCREEN_HEIGHT);

    if (getRotation)
      startPosition.rotation = 90.0;

    return startPosition;
  }

  if (spawn == TOP) {
    startPosition.position.x = (float)randomNumber(SCREEN_WIDTH);
    startPosition.position.y = -5.0;

    if (getRotation)
      startPosition.rotation = 180.0;

    return startPosition;
  }

  if (spawn == BOTTOM) {
    startPosition.position.x = (float)randomNumber(SCREEN_WIDTH);
    startPosition.position.y = (float)SCREEN_HEIGHT + 5.0;

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

  return Vector2Normalize(position);
}

Vector2 getObjectPosition(TexturePro texture) {
  Vector2 position = {
    .x = texture.dest.x,
    .y = texture.dest.y,
  };

  return position;
}

TexturePro buildTexturePro(Vector2 *startPosition, const char *spritePath, Texture2D *prevLoadedSprite) {
  Vector2 position;

  if (startPosition == NULL) {
    position.x = 0.0;
    position.y = 0.0;
  } else {
    position.x = startPosition->x;
    position.y = startPosition->y;
  }

  Texture2D sprite;

  if (prevLoadedSprite == NULL)
    sprite = LoadTexture(spritePath);
  else
    sprite = *prevLoadedSprite;

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
    player->isBoosting = true;
  }

  if (IsKeyUp(KEY_W)) {
    player->isBoosting = false;
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

TexturePro getPlayerFireEffect(TexturePro playerTexture, Texture2D *fireSprite) {
  Vector2 firePosition = getObjectPosition(playerTexture);
  firePosition.x += sin(playerTexture.rotation * DEG2RAD) * PLAYER_FIRE_EFFECT_POSITION_SCALE;
  firePosition.y -= cos(playerTexture.rotation * DEG2RAD) * PLAYER_FIRE_EFFECT_POSITION_SCALE;

  TexturePro fireTexture = buildTexturePro(&firePosition, NULL, fireSprite);
  fireTexture.rotation = playerTexture.rotation;

  return fireTexture;
}

void renderPlayer(Player *player, GameStatus *gameStatus, Texture2D *fireSprite, Texture2D *damageSprite) {
  if (*gameStatus == PLAYER_DEATH_PAUSE) {
    TexturePro damage = buildTexturePro(&playerDeathPosition, NULL, damageSprite);
    renderTexturePro(damage);
    return;
  }

  renderTexturePro(player->texture);
  if (player->isBoosting) {
    renderTexturePro(getPlayerFireEffect(player->texture, fireSprite));
  }
}

void spawnProjectile(Array *projectiles, Vector2 *startPosition, float rotation, bool enemyProjectile) {
  Projectile projectile;

  if (enemyProjectile) {
    projectile.texture = buildTexturePro(startPosition, "./assets/enemy_projectile.png", NULL);
    projectile.enemyProjectile = true;
  } else {
    projectile.texture = buildTexturePro(startPosition, "./assets/projectile.png", NULL);
    projectile.enemyProjectile = false;
  }

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

Vector2 getProjectileStartPosition(Vector2 shooterPos, float shooterRotation) {
  Vector2 position = {
    .x = shooterPos.x,
    .y = shooterPos.y,
  };
  position.x += sin(shooterRotation * DEG2RAD) * PROJECTILE_START_POSITION_SCALE;
  position.y -= cos(shooterRotation * DEG2RAD) * PROJECTILE_START_POSITION_SCALE;

  return position;
}

void moveProjectile(Projectile *projectile, float speed) {
  projectile->texture.dest.x += sin(projectile->texture.rotation * DEG2RAD) * speed;
  projectile->texture.dest.y -= cos(projectile->texture.rotation * DEG2RAD) * speed;
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
    .texture = buildTexturePro(NULL, "./assets/asteroid_big.png", NULL)
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
  TexturePro texture = buildTexturePro(NULL, "./assets/enemy.png", NULL);

  Enemy enemy = {
    .texture = texture,
    .shootTimer = {
      .startTime = GetTime(),
      .lifeTime = 3.0
    } 
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

void resetGameState(Player *player, Array *projectiles, Array *asteroids, Array *enemies)
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

  free(enemies->ptr);
  enemies->ptr = NULL;
  enemies->length = 0;
}

void onPlayerDeath(
  Player *player,
  Array *asteroids,
  Array *projectiles,
  Array *enemies,
  Sounds sounds,
  GameStatus *gameStatus,
  int indexOfElementCollided,
  CollidedObject collidedObject)
{
  if (player->lifes == 1) {
    PlaySound(sounds.explode);
    printf("GAME: game over\n");
    *gameStatus = GAME_OVER;
    playerDeathPosition.x = player->texture.dest.x;
    playerDeathPosition.y = player->texture.dest.y;
    resetGameState(player, projectiles, asteroids, enemies);
    startTimer(&playerDeathTimer, 2.0);
    return;
  }

  player->lifes--;
  playerDeathPosition.x = player->texture.dest.x;
  playerDeathPosition.y = player->texture.dest.y;
  player->texture.dest.x = SCREEN_WIDTH / 2.0;
  player->texture.dest.y = SCREEN_HEIGHT / 2.0;
  *gameStatus = PLAYER_DEATH_PAUSE;
  startTimer(&playerDeathTimer, 2.0);

  if (collidedObject == ASTEROID) {
    deleteElementFromArray(asteroids, indexOfElementCollided);
    PlaySound(sounds.explode);
    return;
  }

  if (collidedObject == PROJECTILE) {
    deleteElementFromArray(projectiles, indexOfElementCollided);
    PlaySound(sounds.explode);
    return;
  }

  if (collidedObject == ENEMY) {
    deleteElementFromArray(enemies, indexOfElementCollided);
    PlaySound(sounds.explode);
    return;
  }
}

void update(
  Player *player,
  Array *projectiles,
  Array *asteroids,
  Array *enemies,
  Timer *asteroidSpawnTimer,
  Timer *enemySpawnTimer,
  Sounds sounds,
  GameStatus *gameStatus)
{
  if (*gameStatus == PLAYER_DEATH_PAUSE) {
    if (isTimerDone(&playerDeathTimer)) {
      *gameStatus = RUNNING;
    }

    return;
  }

  // Input updates
  movePlayer(player, sounds.thrust);
  if (IsKeyPressed(KEY_SPACE)) {
    Vector2 playerPosition = {
      .x = player->texture.dest.x,
      .y = player->texture.dest.y,
    };

    Vector2 projectilePosition = getProjectileStartPosition(playerPosition, player->texture.rotation);
    spawnProjectile(projectiles, &projectilePosition, player->texture.rotation, false);
    PlaySound(sounds.shoot);
  }

  // Scripted updates
  // Projectiles
  for (int i = 0; i < projectiles->length; ++i) {
    Projectile* projectilesData = projectiles->ptr;
    Projectile projectile = projectilesData[i];

    if (checkObjectsCollision(projectile.texture.dest, player->texture.dest) && projectile.enemyProjectile) {
      onPlayerDeath(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        gameStatus,
        i,
        PROJECTILE
      );
      deleteElementFromArray(projectiles, i);
      break;
    }

    if (projectile.enemyProjectile)
      moveProjectile(&projectilesData[i], PROJECTILE_ENEMY_SPEED); 
    else
      moveProjectile(&projectilesData[i], PROJECTILE_PLAYER_SPEED); 

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
      onPlayerDeath(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        gameStatus,
        i,
        ASTEROID
      ); 
      break;
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

    if (checkObjectsCollision(enemy.texture.dest, player->texture.dest)) {
      onPlayerDeath(
        player,
        asteroids,
        projectiles,
        enemies,
        sounds,
        gameStatus,
        i,
        ENEMY
      );
      break;
    }

    for (int j = 0; j < projectiles->length; ++j) {
      Projectile *projectileData = projectiles->ptr;

      if (checkObjectsCollision(projectileData[j].texture.dest, enemiesData[i].texture.dest) && !projectileData[i].enemyProjectile) {
        deleteElementFromArray(enemies, i);
        deleteElementFromArray(projectiles, j);

        player->score += ENEMY_SCORE;
        PlaySound(sounds.asteroidDestroyed);
        break;
      }
    }

    Vector2 pos = {
      .x = enemy.texture.dest.x,
      .y = enemy.texture.dest.y,
    };

    Vector2 playerPos = {
      .x = player->texture.dest.x,
      .y = player->texture.dest.y,
    };

    Vector2 origin = {
      .x = (float)SCREEN_WIDTH / 2.0,
      .y = (float)SCREEN_HEIGHT / 2.0
    };

    Vector2 direction = Vector2Subtract(playerPos, pos); 

    // X and Y needs to be swapped - (0, 0) = TOP LEFT CORNER
    // Negative Y because upwards is to the bottom direction in raylib
    float angle = atan2(direction.x, -direction.y);
    enemiesData[i].texture.rotation = angle * RAD2DEG;

    Vector2 position = moveObjectBasedOnSpawn(
      pos,
      enemy.spawn,
      ENEMY_SPEED,
      false
    );
    enemiesData[i].texture.dest.x = position.x;
    enemiesData[i].texture.dest.y = position.y;

    if (isObjectOutOfBounds(position))
      deleteElementFromArray(enemies, i);

    if (isTimerDone(&enemiesData[i].shootTimer)) {
      Vector2 projectilePos = getProjectileStartPosition(pos, enemy.texture.rotation);
      spawnProjectile(projectiles, &projectilePos, enemy.texture.rotation, true);
      startTimer(&enemiesData[i].shootTimer, 3.0);
      PlaySound(sounds.shoot);
    }
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
  Player *player,
  Array projectiles,
  Array asteroids,
  Array enemies,
  GameStatus *gameStatus,
  StaticAssets assets)
{
  renderPlayer(player, gameStatus, &assets.playerShipFire, &assets.damage);

  char lifeBuf[20];
  char scoreBuf[20];

  snprintf(scoreBuf, 20, "Score: %d", player->score);
  DrawText(scoreBuf, 50, 50, 25, WHITE);

  snprintf(lifeBuf, 20, "Lifes: %d", player->lifes);
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

  GameStatus gameStatus = RUNNING; 

  Sounds sounds = {
    .shoot = LoadSound("./assets/shoot.wav"),
    .explode = LoadSound("./assets/explode.wav"),
    .thrust = LoadSound("./assets/thrust.wav"),
    .asteroidDestroyed = LoadSound("./assets/asteroid.wav")
  };

  StaticAssets assets = {
    .playerShipFire = LoadTexture("./assets/fire.png"),
    .damage = LoadTexture("./assets/damage.png")
  };

  Vector2 playerStartPosition = { 
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  Player player = {
    .texture = buildTexturePro(&playerStartPosition, "./assets/player.png", NULL),
    .speed = 0.0,
    .lifes = 2,
    .score = 0,
    .isBoosting = false,
  };

  Timer asteroidSpawnTimer;
  startTimer(&asteroidSpawnTimer, 2.0);

  Timer enemySpawnTimer;
  double enemyTime = 5.0;
  startTimer(&enemySpawnTimer, 5.0);

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
    if (gameStatus == GAME_OVER && IsKeyPressed(KEY_SPACE)) {
      gameStatus = RUNNING;
    } else {
      update(
        &player,
        &projectiles,
        &asteroids,
        &enemies,
        &asteroidSpawnTimer,
        &enemySpawnTimer,
        sounds,
        &gameStatus
      );
    }

    // Render logic
    BeginDrawing();
      ClearBackground(BLACK);
      if (gameStatus == GAME_OVER) {
        renderGameOver();
      } else {
        render(&player, projectiles, asteroids, enemies, &gameStatus, assets);
      }
    EndDrawing();
  }

  return 0;
}
