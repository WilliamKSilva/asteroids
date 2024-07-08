#include "enemy.h"
#include "common.h"

#include <stdio.h>

void enemy_spawn(Array *enemies) 
{
  const int spawn_limit = RIGHT;

  TexturePro texture = build_texture_pro(NULL, "./assets/enemy.png", NULL);

  Enemy enemy = {
    .texture = texture,
    .shoot_timer = {
      .start_time = GetTime(),
      .life_time = 3.0
    } 
  };

  enemy.spawn = random_number(spawn_limit);

  ObjectStartPosition startPosition = object_start_position_by_spawn(enemy.spawn, true);
  enemy.texture.dest.x = startPosition.position.x;
  enemy.texture.dest.y = startPosition.position.y;

  // Set enemy facing torwards direction based on spawn
  enemy.texture.rotation = startPosition.rotation;

  if (enemies->length == 0) {
    Enemy *ptr = (Enemy*)malloc(sizeof(Enemy));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for enemies\n");
      exit(-1);
    }

    ptr[0] = enemy;
    enemies->ptr = ptr;
    enemies->length++;
  } else {
    enemies->length++;
    Enemy *ptr = (Enemy*)realloc(enemies->ptr, enemies->length * sizeof(Enemy));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for enemies\n");
      exit(-1);
    }

    ptr[enemies->length - 1] = enemy;
    enemies->ptr = ptr;
  }
}

void enemy_move(Enemy *enemy)
{
  Vector2 pos = move_object_by_spawn(object_position(enemy->texture), enemy->spawn, 3.0, false);
  enemy->texture.dest.x = pos.x;
  enemy->texture.dest.y = pos.y;
}
