#include "common.h"

#include "raymath.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

Score score = {
  .enemy = 50,
  .big_asteroid = 100
};

int random_number(int min, int max)
{
  return rand() % (max + 1 - min) + min; 
}

ObjectStartPosition object_start_position_by_spawn(Spawn spawn, bool getRotation) {
  const int screen_width = GetScreenWidth();
  const int screen_height = GetScreenWidth();

  ObjectStartPosition startPosition = {
    .position = {
      .x = 0.0,
      .y = 0.0
    },
    .rotation = 0.0
  };

  printf("%d\n", spawn);

  if (spawn == RIGHT) {
    startPosition.position.x = (float)screen_width + 5.0;
    startPosition.position.y = (float)random_number(100, screen_height);

    if (getRotation)
      startPosition.rotation = 270.0;

    return startPosition;
  }

  if (spawn == LEFT) {
    startPosition.position.x = -5.0;
    startPosition.position.y = (float)random_number(100, screen_height);

    if (getRotation)
      startPosition.rotation = 90.0;

    return startPosition;
  }

  if (spawn == TOP) {
    startPosition.position.x = (float)random_number(100, screen_width);
    startPosition.position.y = -5.0;

    if (getRotation)
      startPosition.rotation = 180.0;

    return startPosition;
  }

  if (spawn == BOTTOM) {
    startPosition.position.x = (float)random_number(100, screen_width);
    startPosition.position.y = (float)screen_height + 5.0;

    if (getRotation)
      startPosition.rotation = 0.0;

    return startPosition;
  }

  return startPosition;
}

Vector2 move_object_by_spawn(Vector2 position, Spawn spawn, float speed, DiagonalMovement movement) {
  switch (spawn) {
    case TOP:
      position.y += speed;

      if (movement == DIAGONAL_LEFT) {
        position.x -= speed;
      }
        
      if (movement == DIAGONAL_RIGHT) {
        position.x += speed;
      }
      break;
    case BOTTOM:
      position.y -= speed;

      if (movement == DIAGONAL_LEFT) {
        position.x -= speed;
        return position;
      }
        
      if (movement == DIAGONAL_RIGHT) {
        position.x += speed;
        return position;
      }
      break;
    case LEFT:
      position.x += speed;

      if (movement == DIAGONAL_LEFT) {
        position.y -= speed;
        return position;
      }
        
      if (movement == DIAGONAL_RIGHT) {
        position.y += speed;
        return position;
      }

      break;
    case RIGHT:
      position.x -= speed;

      if (movement == DIAGONAL_LEFT) {
        position.y -= speed;
      }
        
      if (movement == DIAGONAL_RIGHT) {
        position.y += speed;
      }

      break;
  }

  return position;
}

Vector2 object_position(TexturePro texture) {
  Vector2 position = {
    .x = texture.dest.x,
    .y = texture.dest.y,
  };

  return position;
}

bool object_is_out_of_bounds(Vector2 position) {
  if (position.x > GetScreenWidth() + 50 || position.x < -50)
      return true;

  if (position.y > GetScreenHeight() + 50 || position.y < -50)
      return true;

  return false;
}

bool object_collision_check(Rectangle rec1, Rectangle rec2) {
  return CheckCollisionRecs(rec1, rec2);
}

float object_rotation_torwards_target(Vector2 pos, Vector2 target_pos)
{
  Vector2 direction = Vector2Subtract(target_pos, pos); 
  // X and Y needs to be swapped - (0, 0) = TOP LEFT CORNER
  // Negative Y because upwards is to the bottom direction in raylib
  float angle = atan2(direction.x, -direction.y) * RAD2DEG;

  return angle;
}
