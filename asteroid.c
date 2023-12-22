#include "raylib.h" 
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "asteroid.h"

struct Asteroid* NewAsteroid(Vector2 position, Vector2 speed, enum ASTEROID_TYPE type, enum ASTEROID_MOVE move, enum ASTEROID_DIRECTION direction) {
  struct Asteroid* asteroid = malloc(sizeof(struct Asteroid));
  asteroid->position = position;
  asteroid->speed = speed;

  char cwd[PATH_MAX];
  char* path = getcwd(cwd, sizeof(cwd));
  
  if (path == NULL) {
    perror("Error trying to load meteor asset");
  }

  Texture2D sprite;
  if (asteroid->type == BIG) {
    char* bigMeteorPath = strcat(path, "/assets/meteors/meteor-big.png");
    sprite = LoadTexture(bigMeteorPath);   
  }

  if (asteroid->type == MEDIUM) {
    char* bigMeteorPath = strcat(path, "/assets/meteors/meteor-medium.png");
    sprite = LoadTexture(bigMeteorPath);   
  }

  if (asteroid->type == SMALL) {
    char* bigMeteorPath = strcat(path, "/assets/meteors/meteor-small.png");
    sprite = LoadTexture(bigMeteorPath);   
  }

  asteroid->sprite = sprite;

  return asteroid;
}

void RenderAsteroid(struct Asteroid *asteroid) {
  DrawCircle(asteroid->collisionArea.centerX, asteroid->collisionArea.centerY, asteroid->collisionArea.radius, asteroid->collisionArea.color);
  DrawTexture(asteroid->sprite, asteroid->position.x, asteroid->position.y, WHITE);
}

void MoveAsteroid(struct Asteroid *asteroid) {
  Vector2 velocity = Vector2Zero();
  velocity = asteroid->position;

  if (asteroid->direction == UP) {
    velocity.y -= asteroid->speed.y;

    printf("%f", asteroid->position.y);
    printf("\n");
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
