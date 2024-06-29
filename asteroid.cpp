#include "asteroid.h"

Asteroid::Asteroid(Vector2 position) : GameObject(position, "./assets/asteroid.png")
{
  spawn = (Spawns)randomNumber(spawnLimit);

  Vector2 initialPosition;

  if (spawn == TOP)
  {
    int randomX = randomNumber(1890);
    initialPosition.x = randomX;
    initialPosition.y = -5;
  }

  if (spawn == BOTTOM)
  {
    int randomX = randomNumber(1890);
    initialPosition.x = randomX;
    initialPosition.y = 1085;
  }

  if (spawn == LEFT)
  {
    int randomY = randomNumber(1050);
    initialPosition.y = randomY;
    initialPosition.x = -5;
  }

  if (spawn == RIGHT)
  {
    int randomY = randomNumber(1050);
    initialPosition.y = randomY;
    initialPosition.x = 1925;
  }

  updatePosition(initialPosition);
};

void Asteroid::move()
{
  Vector2 currentPosition = position;

  if (spawn == TOP)
  {
    currentPosition.y += maxSpeed;
  }

  if (spawn == BOTTOM)
  {
    currentPosition.y -= maxSpeed;
  }

  if (spawn == LEFT)
  {
    currentPosition.x += maxSpeed;
  }

  if (spawn == RIGHT)
  {
    currentPosition.x -= maxSpeed;
  }

  updatePosition(currentPosition);
}