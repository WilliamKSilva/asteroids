#ifndef ENEMY_H
#define ENEMY_H

#include "texture_pro.h"
#include "array.h"
#include "common.h"
#include "timer.h"

typedef struct {
  TexturePro texture;
  Spawn spawn;
  Timer shoot_timer; 
} Enemy;

void enemy_spawn(Array *enemies);
void enemy_move(Enemy *enemy);

#endif
