#ifndef GAME
#define GAME

#include "raylib.h"

typedef struct {
  int centerX;
  int centerY;
  float radius;
  Color color;
} Circle;

typedef struct {
  Rectangle sourceRec;
  Rectangle destRec;
  Vector2 origin;
  float rotation;
} TexturePro;

typedef struct {
  Vector2 position;
  Texture2D *sprite;
  TexturePro *texturePro;
} GameObject;

typedef struct {
  GameObject *gameObject;
  float speed;
} Player;

typedef enum {
  TOP,
  BOTTOM,
  LEFT,
  RIGHT
} AsteroidSpawn;

typedef struct {
  GameObject *gameObject;
  AsteroidSpawn spawn;
} Asteroid;

typedef struct {
  GameObject *gameObject;
} Projectile;

#endif
