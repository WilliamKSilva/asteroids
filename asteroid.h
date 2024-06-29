#ifndef ASTEROID
#define ASTEROID

#include <iostream>
#include "gameObject.h"
#include "raylib.h"
#include "random.h"

using namespace std;

class Asteroid : public GameObject
{
public:
	float maxSpeed = 7.0;
	enum Spawns
	{
		TOP,
		BOTTOM,
		LEFT,
		RIGHT
	};

	Spawns spawn;
	int spawnLimit = RIGHT;

	Asteroid(Vector2 position);

	void move();
};

#endif