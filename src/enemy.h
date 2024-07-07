#ifndef ENEMY_H
#define ENEMY_H

#include "texture_pro.h"
#include "common.h"
#include "timer.h"

typedef struct {
  TexturePro texture;
  Spawn spawn;
  Timer shootTimer; 
} Enemy;

void enemy_spawn(Array *enemies);
void enemy_move(Enemy *enemy);

#endif
