#ifndef PROJECTILE
#define PROJECTILE

#include "math.h"
#include "gameObject.hpp"
#include "player.hpp"
#include <vector>

using namespace std;

class Projectile : public GameObject
{
public:
	float speed = 20.0;

	Projectile(Vector2 position) : GameObject(position, "./assets/projectile.png"){};

	static void spawn(vector<Projectile> &projectiles, Projectile projectile, Player player) {
		Vector2 projectilePosition = Projectile::getInitialPosition(player.position, player.texturePro.rotation);
		projectile.updatePosition(projectilePosition);
		projectile.texturePro.rotation = player.texturePro.rotation;
		projectiles.push_back(projectile);
	}

	static Vector2 getInitialPosition(Vector2 playerPosition, float playerRotation)
	{
		float xAxisPosition = playerPosition.x + (sin(playerRotation * DEG2RAD) * 70);
		float yAxisPosition = playerPosition.y - (cos(playerRotation * DEG2RAD) * 70);
		return {
				.x = xAxisPosition,
				.y = yAxisPosition};
	}

	void move()
	{
		Vector2 newPosition = position;
		newPosition.x += (sin(texturePro.rotation * DEG2RAD) * speed);
		newPosition.y -= (cos(texturePro.rotation * DEG2RAD) * speed);
		updatePosition(newPosition);
	}
};

#endif