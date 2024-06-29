#include "player.h"

Player::Player(Vector2 position) : GameObject(position, "./assets/player.png"){};

void Player::move()
{
  if (IsKeyDown(KEY_A))
  {
    texturePro.rotation -= 5;
  }

  if (IsKeyDown(KEY_D))
  {
    texturePro.rotation += 5;
  }

  if (IsKeyDown(KEY_W))
  {
    if (speed < playerMaxSpeed)
    {
      speed += playerImpulseSpeed;
    }

    Vector2 newPosition = position;
    newPosition.x += (sin(texturePro.rotation * DEG2RAD) * speed);
    newPosition.y -= (cos(texturePro.rotation * DEG2RAD) * speed);
    updatePosition(newPosition);
  }

  if (IsKeyUp(KEY_W))
  {
    if (speed > 0)
      speed -= playerDragSpeed;

    if (speed < 0)
    {
      speed = 0.0;
      return;
    }

    Vector2 newPosition = position;
    newPosition.x += (sin(texturePro.rotation * DEG2RAD) * speed);
    newPosition.y -= (cos(texturePro.rotation * DEG2RAD) * speed);
    updatePosition(newPosition);
  }
}

void Player::resetPosition()
{
  Vector2 initialPosition;
  initialPosition.x = 1920 / 2.0;
  initialPosition.y = 1080 / 2.0;
  updatePosition(initialPosition);
}

void Player::renderStats()
{
  scoreText.content.append(to_string(score));
  lifesText.content.append(to_string(lifes));

  scoreText.render();
  lifesText.render();
}