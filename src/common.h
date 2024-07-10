#ifndef GAME_H
#define GAME_H 

#include "raylib.h"
#include "texture_pro.h"

typedef enum {
  PACIFIC,
  REGULAR
} GameMode;

typedef enum {
  MENU,
  GAME_OVER,
  PLAYER_DEATH_PAUSE,
  RUNNING,
} GameStatus;

typedef enum {
  ASTEROID,
  PROJECTILE,
  ENEMY
} CollidedObject;

typedef enum {
  NONE,
  DIAGONAL_RIGHT,
  DIAGONAL_LEFT,
} DiagonalMovement;

typedef struct {
  Sound explode;
  Sound shoot;
  Sound thrust;
  Sound destroyed;
} Sounds;

typedef struct {
  Texture2D playerShipFire;
  Texture2D damage;
} StaticAssets;

typedef enum {
  TOP,
  BOTTOM,
  LEFT,
  RIGHT
} Spawn;

typedef struct {
  const int big_asteroid;
  const int enemy;
} Score;

typedef struct {
  Vector2 position;
  float rotation; // Object facing torwards rotation based on spawn
} ObjectStartPosition;

extern Score score;
ObjectStartPosition object_start_position_by_spawn(Spawn spawn, bool getRotation);
Vector2 move_object_by_spawn(Vector2 currentPosition, Spawn spawn, float speed, DiagonalMovement movement);
Vector2 object_position(TexturePro texture);
int random_number(int min, int max);
bool object_is_out_of_bounds(Vector2 position);
bool object_collision_check(Rectangle rec1, Rectangle rec2);
float object_rotation_torwards_target(Vector2 pos, Vector2 targetPos);
#endif