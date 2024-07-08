#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include "texture_pro.h"
#include "array.h"

typedef struct {
  TexturePro texture;
  bool is_enemy_projectile;
} Projectile;

Vector2 projectile_start_position(Vector2 shooter_pos, float shooter_rotation);
void projectile_move(Projectile *projectile, float speed);
void projectile_spawn(Array *projectiles, Vector2 *start_position, float rotation, bool enemy_projectile);

#endif
