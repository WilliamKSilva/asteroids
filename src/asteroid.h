#ifndef ASTEROID_H
#define ASTEROID_H

#include "common.h"
#include "texture_pro.h"

typedef struct {
  TexturePro texture;
  Spawn spawn; 
  bool diagonal_move; 
} Asteroid;

void asteroid_spawn(Array *asteroids);

#endif
