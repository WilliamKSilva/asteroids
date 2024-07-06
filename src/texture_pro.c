
#include "texture_pro.h"

#include <stdlib.h>
#include "common.h"
#include "math.h"

const int fire_effect_position_scale = -55;

void render_texture_pro(TexturePro texture) {
  DrawTexturePro(
    texture.sprite,
    texture.source,
    texture.dest,
    texture.origin,
    texture.rotation,
    texture.color 
  );
}

TexturePro build_texture_pro(Vector2 *startPosition, const char *spritePath, Texture2D *prevLoadedSprite) {
  Vector2 position;

  if (startPosition == NULL) {
    position.x = 0.0;
    position.y = 0.0;
  } else {
    position.x = startPosition->x;
    position.y = startPosition->y;
  }

  Texture2D sprite;

  if (prevLoadedSprite == NULL)
    sprite = LoadTexture(spritePath);
  else
    sprite = *prevLoadedSprite;

  TexturePro texture = {
    .color = WHITE,
    .source = {
      .x = 0.0,
      .y = 0.0,
      .width = (float)sprite.width,
      .height = (float)sprite.height
    },
    .dest = {
      .x = position.x,
      .y = position.y,
      .width = (float)sprite.width,
      .height = (float)sprite.height
    },
    .sprite = sprite,
    .origin = {
      .x =  (float)sprite.width / 2.0,
      .y = (float)sprite.height / 2.0
    },
    .rotation = 0.0
  };

  return texture;
}

TexturePro build_fire_effect_texture(TexturePro playerTexture, Texture2D *fireSprite) {
  Vector2 firePosition = object_position(playerTexture);
  firePosition.x += sin(playerTexture.rotation * DEG2RAD) * fire_effect_position_scale;
  firePosition.y -= cos(playerTexture.rotation * DEG2RAD) * fire_effect_position_scale;

  TexturePro fireTexture = build_texture_pro(&firePosition, NULL, fireSprite);
  fireTexture.rotation = playerTexture.rotation;

  return fireTexture;
}
