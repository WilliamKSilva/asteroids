#include "projectile.h"

#include <stdio.h>
#include "math.h"

const float projectile_start_position_scale = 0;

Vector2 projectile_start_position(Vector2 shooter_pos, float shooter_rotation) 
{
  Vector2 position = {
    .x = shooter_pos.x,
    .y = shooter_pos.y,
  };
  position.x += sin(shooter_rotation * DEG2RAD) * projectile_start_position_scale;
  position.y -= cos(shooter_rotation * DEG2RAD) * projectile_start_position_scale;

  return position;
}

void projectile_move(Projectile *projectile, float speed) 
{
  projectile->texture.dest.x += sin(projectile->texture.rotation * DEG2RAD) * speed;
  projectile->texture.dest.y -= cos(projectile->texture.rotation * DEG2RAD) * speed;
}

void projectile_spawn(Array *projectiles, Vector2 *start_position, float rotation, bool enemy_projectile) 
{
  Projectile projectile;

  if (enemy_projectile) {
    projectile.texture = build_texture_pro(start_position, "./assets/enemy_projectile.png", NULL);
    projectile.is_enemy_projectile = true;
  } else {
    projectile.texture = build_texture_pro(start_position, "./assets/projectile.png", NULL);
    projectile.is_enemy_projectile = false;
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
