#include "projectile.h"

Projectile::Projectile(Vector2 position) : GameObject(position, "./assets/projectile.png"){};

void Projectile::spawn(vector<Projectile> &projectiles, Projectile projectile, Player player)
{
  Vector2 projectilePosition = Projectile::getInitialPosition(player.position, player.texturePro.rotation);
  projectile.updatePosition(projectilePosition);
  projectile.texturePro.rotation = player.texturePro.rotation;
  projectiles.push_back(projectile);
}

Vector2 Projectile::getInitialPosition(Vector2 playerPosition, float playerRotation)
{
  float xAxisPosition = playerPosition.x + (sin(playerRotation * DEG2RAD) * 70);
  float yAxisPosition = playerPosition.y - (cos(playerRotation * DEG2RAD) * 70);
  return {
      .x = xAxisPosition,
      .y = yAxisPosition};
}

void Projectile::move()
{
  Vector2 newPosition = position;
  newPosition.x += (sin(texturePro.rotation * DEG2RAD) * speed);
  newPosition.y -= (cos(texturePro.rotation * DEG2RAD) * speed);
  updatePosition(newPosition);
}