#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include "texture_pro.h"
#include "array.h"

typedef struct {
  TexturePro texture;
  bool enemyProjectile;
} Projectile;

Vector2 projectile_start_position(Vector2 shooterPos, float shooterRotation);
void projectile_move(Projectile *projectile, float speed);
void projectile_spawn(Array *projectiles, Vector2 *startPosition, float rotation, bool enemyProjectile);

#endif
