#ifndef PLAYER
#define PLAYER

#include "raylib.h"
#include "math.h"
#include "gameObject.hpp"
#include <iostream>

using namespace std;

class Player : public GameObject
{
public:
	float playerMaxSpeed = 10.0;
	float playerImpulseSpeed = 0.6;
	float playerDragSpeed = 0.2;

	float speed = 0.0;

	Player(Vector2 position) : GameObject(position, "./assets/player.png"){};

	void move()
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

	void resetPosition() {
		Vector2 initialPosition;
		initialPosition.x = 1920 / 2.0;
		initialPosition.y = 1080 / 2.0;
		updatePosition(initialPosition);
	}
};

#endif