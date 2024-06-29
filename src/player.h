#ifndef PLAYER
#define PLAYER

#include <iostream>
#include "raylib.h"
#include "math.h"
#include "gameObject.h"
#include "text.h"

using namespace std;

class Player : public GameObject
{
public:
	float playerMaxSpeed = 10.0;
	float playerImpulseSpeed = 0.6;
	float playerDragSpeed = 0.2;

	float speed = 0.0;
	size_t score = 0;
	size_t lifes = 5;

	Text scoreText = Text(
		"Score: ",
		{
			.x = 20,
			.y = 20,
		},
		21,
		WHITE);

	Text lifesText = Text(
		"Lifes: ",
		{
			.x = 20,
			.y = 40,
		},
		21,
		RED);

	Player(Vector2 position);
	void move();
	void resetPosition();
	void renderStats();
};

#endif