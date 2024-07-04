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
  float speed;
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
} Asteroid;

typedef struct {
  void *ptr;
  int length;
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

void movePlayer(Player *player) {
  if (IsKeyDown(KEY_A))
    player->texture.rotation -= PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_D))
    player->texture.rotation += PLAYER_ROTATION_SPEED;

  if (IsKeyDown(KEY_W)) {
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

void deleteElementFromArray(Array *array, int indexToDelete) {
  if (array->length == 1) {
    free(array->ptr);
    array->ptr = NULL;
    array->length = 0;
    printf("INFO: array with one element freed\n");
    return;
  }

  array->length--;
  memmove(array->ptr + indexToDelete, array->ptr+indexToDelete+1, (array->length - indexToDelete) * sizeof array->ptr);

  printf("INFO: object deleted\n");
}

void spawnAsteroid(Array *asteroids) {
  Asteroid asteroid = {
    .texture = buildTexturePro(NULL, "./assets/asteroid_big.png")
  };

  asteroid.spawn = randomNumber(asteroidSpawnLimit);

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
  if (asteroid->spawn == TOP)
    asteroid->texture.dest.y += ASTEROID_SPEED;

  if (asteroid->spawn == BOTTOM)
    asteroid->texture.dest.y -= ASTEROID_SPEED;

  if (asteroid->spawn == LEFT)
    asteroid->texture.dest.y += ASTEROID_SPEED;

  if (asteroid->spawn == RIGHT)
    asteroid->texture.dest.y -= ASTEROID_SPEED;
}

void update(Player *player, Array *projectiles, Array *asteroids, Timer *asteroidSpawnTimer, Sound shootSound) {
  // Input updates
  movePlayer(player);
  shootProjectile(projectiles, *player, shootSound);

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

  for (int i = 0; i < projectiles.length; ++i) {
    Projectile* ptr = (Projectile*)projectiles.ptr;
    renderTexturePro(ptr[i].texture); 
  }

  for (int i = 0; i < asteroids.length; ++i) {
    Asteroid* asteroidsData = (Asteroid*)asteroids.ptr;
    renderTexturePro(asteroidsData[i].texture); 
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
  InitAudioDevice();
  SetTargetFPS(60);
  srand(time(NULL));

  Sound shootSound = LoadSound("./assets/shoot.wav"); 

  Vector2 playerStartPosition = { 
    .x = (float)SCREEN_WIDTH / 2.0,
    .y = (float)SCREEN_HEIGHT / 2.0
  };
  Player player = {
    .texture = buildTexturePro(&playerStartPosition, "./assets/player.png"),
    .speed = 0.0
  };

  Timer asteroidSpawnTimer;
  startTimer(&asteroidSpawnTimer, 4.0);

  Array projectiles = {
    .ptr = NULL,
    .length = 0,
  };

  Array asteroids = {
    .ptr = NULL,
    .length = 0,
  };

  while (!WindowShouldClose()) {
    update(&player, &projectiles, &asteroids, &asteroidSpawnTimer, shootSound);

    BeginDrawing(); 
      ClearBackground(BLACK);
      render(player, projectiles, asteroids);
    EndDrawing();
  }

  return 0;
}
