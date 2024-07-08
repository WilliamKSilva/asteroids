#include "asteroid.h"

#include <raymath.h>
#include <stdio.h>
#include "common.h"

void asteroid_spawn(Array *asteroids) 
{
  const int spawn_limit = RIGHT;

  Asteroid asteroid = {
    .texture = build_texture_pro(NULL, "./assets/asteroid_big.png", NULL)
  };

  asteroid.spawn = random_number(spawn_limit + 1);

  // 0 or 1 = false or true
  asteroid.diagonal_move = random_number(2);

  ObjectStartPosition startPosition = object_start_position_by_spawn(asteroid.spawn, false);

  asteroid.texture.dest.x = startPosition.position.x;
  asteroid.texture.dest.y = startPosition.position.y;

  if (asteroids->length == 0) {
    Asteroid *ptr = (Asteroid*)malloc(sizeof(Asteroid));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for asteroids\n");
      exit(-1);
    }

    ptr[0] = asteroid;
    asteroids->ptr = ptr;
    asteroids->length++;
  } else {
    asteroids->length++;
    Asteroid *ptr = (Asteroid*)realloc(asteroids->ptr, asteroids->length * sizeof(Asteroid));

    if (ptr == NULL) {
      printf("INFO: Error trying to allocate memory for asteroids\n");
      exit(-1);
    }

    ptr[asteroids->length - 1] = asteroid;
    asteroids->ptr = ptr;
  }
}
