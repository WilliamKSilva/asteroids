#include "projectile.h"

#include <stdio.h>
#include "math.h"

const float projectile_start_position_scale = 0;

Vector2 projectile_start_position(Vector2 shooterPos, float shooterRotation) {
  Vector2 position = {
    .x = shooterPos.x,
    .y = shooterPos.y,
  };
  position.x += sin(shooterRotation * DEG2RAD) * projectile_start_position_scale;
  position.y -= cos(shooterRotation * DEG2RAD) * projectile_start_position_scale;

  return position;
}

void projectile_move(Projectile *projectile, float speed) {
  projectile->texture.dest.x += sin(projectile->texture.rotation * DEG2RAD) * speed;
  projectile->texture.dest.y -= cos(projectile->texture.rotation * DEG2RAD) * speed;
}

void projectile_spawn(Array *projectiles, Vector2 *startPosition, float rotation, bool enemyProjectile) {
  Projectile projectile;

  if (enemyProjectile) {
    projectile.texture = build_texture_pro(startPosition, "./assets/enemy_projectile.png", NULL);
    projectile.enemyProjectile = true;
  } else {
    projectile.texture = build_texture_pro(startPosition, "./assets/projectile.png", NULL);
    projectile.enemyProjectile = false;
  }

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
