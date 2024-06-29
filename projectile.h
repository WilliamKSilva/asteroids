#ifndef PROJECTILE
#define PROJECTILE

#include "math.h"
#include "gameObject.h"
#include "player.h"
#include <vector>

using namespace std;

class Projectile : public GameObject
{
public:
	float speed = 20.0;

	Projectile(Vector2 position);
	static void spawn(vector<Projectile> &projectiles, Projectile projectile, Player player);
	static Vector2 getInitialPosition(Vector2 playerPosition, float playerRotation);
	void move();
};

#endif