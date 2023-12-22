#ifndef _ASTEROIDH_
#define _ASTEROIDH_

#include "raylib.h"
#include "collision_area.h"

enum ASTEROID_TYPE {BIG, MEDIUM, SMALL};
enum ASTEROID_MOVE {DIAGONAL_LEFT, DIAGONAL_RIGHT};
enum ASTEROID_DIRECTION {UP, DOWN};

struct Asteroid {
  Vector2 position;
  Vector2 speed;
  Texture2D sprite;
  struct CircleCollisionArea collisionArea;
  enum ASTEROID_TYPE type;
  enum ASTEROID_MOVE move;
  enum ASTEROID_DIRECTION direction;
};

struct Asteroid* NewAsteroid(Vector2 position, Vector2 speed, enum ASTEROID_TYPE, enum ASTEROID_MOVE, enum ASTEROID_DIRECTION);
void RenderAsteroid(struct Asteroid *asteroid);
void MoveAsteroid(struct Asteroid *asteroid);
#endif
