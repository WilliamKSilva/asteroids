#include "asteroid.h"
#include "raylib.h"
#include "raymath.h"
#include "resolution.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Asteroid *NewAsteroid(Vector2 position, Vector2 speed,
                             enum ASTEROID_TYPE type, enum ASTEROID_MOVE move,
                             enum ASTEROID_DIRECTION direction) {
  struct Asteroid *asteroid = malloc(sizeof(struct Asteroid));
  asteroid->position = position;
  asteroid->speed = speed;

  Texture2D sprite = GetAsteroidSprite(asteroid->type);
  asteroid->sprite = sprite;

  return asteroid;
}

void RenderAsteroid(struct Asteroid *asteroid) {
  DrawCircle(asteroid->collisionArea.centerX, asteroid->collisionArea.centerY,
             asteroid->collisionArea.radius, asteroid->collisionArea.color);
  DrawTexture(asteroid->sprite, asteroid->position.x, asteroid->position.y,
              WHITE);
}

void MoveAsteroid(struct Asteroid *asteroid) {
  Vector2 velocity = Vector2Zero();
  velocity = asteroid->position;

  if (asteroid->direction == UP) {
    velocity.y -= asteroid->speed.y;
  } else {
    velocity.y += asteroid->speed.y;
  }

  if (asteroid->move == DIAGONAL_LEFT) {
    velocity.x -= asteroid->speed.y;
  } else {
    velocity.x += asteroid->speed.y;
  }

  asteroid->position = velocity;
}

int RandomInt(int min, int max) {
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

Texture2D GetAsteroidSprite(enum ASTEROID_TYPE type) {
  char cwd[PATH_MAX];
  char *path = getcwd(cwd, sizeof(cwd));

  if (path == NULL) {
    perror("Error trying to load meteor asset");
  }

  Texture2D sprite;
  if (type == BIG) {
    char *bigMeteorPath = strcat(path, "/assets/meteors/meteor-big.png");
    sprite = LoadTexture(bigMeteorPath);
  }

  if (type == MEDIUM) {
    char *bigMeteorPath = strcat(path, "/assets/meteors/meteor-medium.png");
    sprite = LoadTexture(bigMeteorPath);
  }

  if (type == SMALL) {
    char *bigMeteorPath = strcat(path, "/assets/meteors/meteor-small.png");
    sprite = LoadTexture(bigMeteorPath);
  }

  return sprite;
}

enum ASTEROID_MOVE GetRandomAsteroidMove() {
  enum ASTEROID_MOVE asteroidMove = DIAGONAL_LEFT;
  int randomMove = RandomInt(DIAGONAL_LEFT, DIAGONAL_RIGHT);

  if (randomMove == DIAGONAL_LEFT) {
    asteroidMove = DIAGONAL_LEFT;
  }

  if (randomMove == DIAGONAL_RIGHT) {
    asteroidMove = DIAGONAL_RIGHT;
  }

  return asteroidMove;
}

struct Asteroid *CreateInitialAsteroids() {
  struct Asteroid *asteroids = malloc(sizeof(struct Asteroid) * 6);

  for (int i = 0; i < 6; i++) {
    Vector2 asteroidPosition = {SCREEN_WIDTH / 2.0 + 200, SCREEN_HEIGHT};
    Vector2 asteroidSpeed = {2.0f, 2.0f};
    int randomType = RandomInt(SMALL, BIG);
    struct Asteroid asteroid = {asteroidPosition, asteroidSpeed, BIG,
                                GetRandomAsteroidMove(), UP};
    struct CircleCollisionArea asteroidCollisionArea =
        NewCollisionArea(asteroidPosition.x, asteroidPosition.y, 50, BLACK);
    asteroid.collisionArea = asteroidCollisionArea;
    Texture2D sprite = GetAsteroidSprite(BIG);
    asteroid.sprite = sprite;
    asteroids[i] = asteroid;
  }

  return asteroids;
}
