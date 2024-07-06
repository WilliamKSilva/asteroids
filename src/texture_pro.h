#ifndef TEXTURE_PRO_H 
#define TEXTURE_PRO_H

#include "raylib.h"

typedef struct {
  Texture2D sprite;
  Rectangle source;
  Rectangle dest;
  Vector2 origin;
  float rotation;
  Color color;
} TexturePro;

void render_texture_pro(TexturePro texture);
TexturePro build_texture_pro(Vector2 *startPosition, const char *spritePath, Texture2D *prevLoadedSprite);
TexturePro build_fire_effect_texture(TexturePro playerTexture, Texture2D *fireSprite);

#endif
