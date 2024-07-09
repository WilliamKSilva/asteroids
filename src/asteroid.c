#include "asteroid.h"

#include <raymath.h>
#include <stdio.h>
#include "common.h"

Asteroid asteroid_build_big()
{
  const int spawn_limit = RIGHT;
  const int asteroid_movement_limit = DIAGONAL_LEFT;

  Asteroid asteroid = {
    .texture = build_texture_pro(NULL, "./assets/asteroid_big.png", NULL)
  };

  ObjectStartPosition start_pos = object_start_position_by_spawn(asteroid.spawn, false);

  asteroid.spawn = random_number(spawn_limit + 1);
  if (start_pos.position.x > 400 && start_pos.position.x < 1000 && start_pos.position.y > 400 && start_pos.position.y < 1000)
    asteroid.diagonal_movement = random_number(asteroid_movement_limit + 1);
  else
    asteroid.diagonal_movement = NONE;

  asteroid.size = BIG;

  asteroid.texture.dest.x = start_pos.position.x;
  asteroid.texture.dest.y = start_pos.position.y;

  return asteroid;
}

Asteroid asteroid_build_small(Asteroid asteroid_big, DiagonalMovement movement) 
{ 

  Asteroid asteroid = {
    .texture = build_texture_pro(NULL, "./assets/asteroid_small.png", NULL)
  };

  asteroid.diagonal_movement = movement;
  asteroid.spawn = asteroid_big.spawn;
  asteroid.size = SMALL;

  Vector2 start_pos = object_position(asteroid_big.texture);

  asteroid.texture.dest.x = start_pos.x;
  asteroid.texture.dest.y = start_pos.y;

  return asteroid;
}

void asteroid_spawn(Array *asteroids, Asteroid asteroid) 
{
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
