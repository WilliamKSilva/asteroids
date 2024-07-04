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

#define BIG_ASTEROID_SCORE 20

// TODO: draw my own assets

typedef struct {
  Sound explode;
  Sound shoot;
  Sound thrust;
} Sounds;

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
} AsteroidSpawn;

typedef struct {
  TexturePro texture;
  AsteroidSpawn spawn; 
  bool diagonalMove; 
} Asteroid;

typedef struct {
  void *ptr;
  int length;
  size_t itemSize;
} Array;

const int asteroidSpawnLimit = RIGHT; 

int randomNumber(int limit) {
  return rand() % limit;
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
  if (position.x > SCREEN_WIDTH || position.x < 0)
      return true;

  if (position.y > SCREEN_HEIGHT || position.y < 0)
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

  asteroid.spawn = randomNumber(asteroidSpawnLimit + 1);

  // 0 or 1 = false or true
  asteroid.diagonalMove = randomNumber(2);

  Vector2 position;
  if (asteroid.spawn == RIGHT) {
    position.x = SCREEN_WIDTH + 5;
    position.y = randomNumber(SCREEN_HEIGHT);
  }

  if (asteroid.spawn == LEFT) {
    position.x = -5;
    position.y = randomNumber(SCREEN_HEIGHT);
  }

  if (asteroid.spawn == TOP) {
    position.x = randomNumber(SCREEN_WIDTH);
    position.y = -5;
  }

  if (asteroid.spawn == BOTTOM) {
    position.x = randomNumber(SCREEN_WIDTH);
    position.y = SCREEN_HEIGHT + 5;
  }

  asteroid.texture.dest.x = position.x;
  asteroid.texture.dest.y = position.y;

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

void moveAsteroid(Asteroid *asteroid) {
  if (asteroid->spawn == TOP) {
    asteroid->texture.dest.y += ASTEROID_SPEED;

    if (asteroid->diagonalMove)
      asteroid->texture.dest.x += ASTEROID_SPEED;
  }

  if (asteroid->spawn == BOTTOM) {
    asteroid->texture.dest.y -= ASTEROID_SPEED;

    if (asteroid->diagonalMove)
      asteroid->texture.dest.x += ASTEROID_SPEED;
  }

  if (asteroid->spawn == LEFT) {
    asteroid->texture.dest.x += ASTEROID_SPEED;

    if (asteroid->diagonalMove)
      asteroid->texture.dest.y += ASTEROID_SPEED;
  }

  if (asteroid->spawn == RIGHT) {
    asteroid->texture.dest.x -= ASTEROID_SPEED;

    if (asteroid->diagonalMove)
      asteroid->texture.dest.y += ASTEROID_SPEED;
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
  Timer *asteroidSpawnTimer,
  Sounds sounds,
  bool *isGameRunning)
{
  // Input updates
  movePlayer(player, sounds.thrust);
  shootProjectile(projectiles, *player, sounds.shoot);

  // Scripted updates
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

  for (int i = 0; i < asteroids->length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)asteroids->ptr;

    moveAsteroid(&asteroidsData[i]);

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
        break;
      }
    }
  }

  if (isTimerDone(asteroidSpawnTimer)) {
    // Spawn new Asteroid 
    spawnAsteroid(asteroids);
    
    // Restart timer
    startTimer(asteroidSpawnTimer, asteroidSpawnTimer->lifeTime);
  }
}

void render(Player player, Array projectiles, Array asteroids) {
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
    .thrust = LoadSound("./assets/thrust.wav")
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
  startTimer(&asteroidSpawnTimer, 3.0);

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

  while (!WindowShouldClose()) {
    // Update logic 
    if (isGameRunning) {
      update(&player, &projectiles, &asteroids, &asteroidSpawnTimer, sounds, &isGameRunning);
    } else {
      if (IsKeyPressed(KEY_SPACE)) {
        isGameRunning = true;
      }
    }

    // Render logic
    BeginDrawing();
      ClearBackground(BLACK);
      if (isGameRunning) {
        render(player, projectiles, asteroids);
      } else {
        renderGameOver();
      }
    EndDrawing();
  }

  return 0;
}
