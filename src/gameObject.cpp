#include "gameObject.h"

GameObject::GameObject(Vector2 position, const char *spritePath)
{
  this->position = position;
  this->sprite = LoadTexture(spritePath);

  TexturePro texturePro;
  texturePro.destRec = {
      .x = position.x,
      .y = position.y,
      .width = (float)this->sprite.width,
      .height = (float)this->sprite.height,
  };
  texturePro.origin = {
      .x = (float)sprite.width / 2,
      .y = (float)sprite.height / 2};
  texturePro.rotation = 0.0;
  texturePro.sourceRec = {
      .x = 0.0,
      .y = 0.0,
      .width = (float)this->sprite.width,
      .height = (float)this->sprite.height,
  };

  this->texturePro = texturePro;
};

void GameObject::render()
{
  DrawTexturePro(
      sprite,
      texturePro.sourceRec,
      texturePro.destRec,
      texturePro.origin,
      texturePro.rotation,
      WHITE);
}

void GameObject::updatePosition(Vector2 position)
{
  this->position = position;
  this->texturePro.destRec.x = position.x;
  this->texturePro.destRec.y = position.y;
}

bool GameObject::isOutOfBounds()
{
  return position.x > 1920 ||
         position.x < 0 ||
         position.y > 1080 ||
         position.y < 0;
}

bool GameObject::checkCollision(Rectangle gameObject)
{
  return CheckCollisionRecs(texturePro.destRec, gameObject);
}