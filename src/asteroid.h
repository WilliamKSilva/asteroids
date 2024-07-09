#ifndef ASTEROID_H
#define ASTEROID_H

#include "common.h"
#include "array.h"
#include "texture_pro.h"

typedef enum {
  SMALL,
  BIG,
} AsteroidSize;

typedef struct {
  TexturePro texture;
  Spawn spawn;
  AsteroidSize size;
  DiagonalMovement diagonal_movement; 
} Asteroid;

void asteroid_spawn(Array *asteroids, Asteroid asteroid);
void asteroid_move(Asteroid *asteroid);
Asteroid asteroid_build_big();
Asteroid asteroid_build_small(Asteroid asteroid_big, DiagonalMovement movementbig_aste);

#endif
