#include <iostream>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "timer.h"
#include <memory>
#include <vector>
#include "gameObject.hpp"
#include "player.hpp"
#include "asteroid.hpp"
#include "projectile.hpp"

using namespace std;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

void update(bool &isGameRunning, Player &player, vector<Asteroid> &asteroids, vector<Projectile> &projectiles, Timer &asteroidSpawnTimer)
{
	/* Player */
	player.move();
	if (IsKeyPressed(KEY_SPACE))
	{
		Projectile projectile = Projectile({.x = 0.0, .y = 0.0});
		Projectile::spawn(projectiles, projectile, player);
	}

	/* Asteroids */
	for (int i = 0; i < asteroids.size(); i++)
	{
		Asteroid &asteroid = asteroids[i];
		asteroid.move();

		if (asteroid.isOutOfBounds()) {
			asteroids.erase(asteroids.begin() + i);
		}
	}

	/* Projectiles */
	for (int i = 0; i < projectiles.size(); i++)
	{
		Projectile &projectile = projectiles[i];
		projectile.move();

		for (int j = 0; j < asteroids.size(); j++)
		{
			Asteroid asteroid = asteroids[j];
			bool hasCollidedWithAsteroid = projectile.checkCollision(asteroid.texturePro.destRec);

			if (hasCollidedWithAsteroid) {
				projectiles.erase(projectiles.begin() + i);
				asteroids.erase(asteroids.begin() + j);
			}
		}

		if (projectile.isOutOfBounds()) {
			projectiles.erase(projectiles.begin() + i);
		}
	}

	/* Spawns */
	if (asteroidSpawnTimer.isDone())
	{
		asteroidSpawnTimer.reset(3.0);

		Asteroid asteroid = Asteroid({.x = 0.0, .y = 0.0});
		asteroids.push_back(asteroid);

		asteroidSpawnTimer.start();
	}
}

void render(Player player, vector<Asteroid> &asteroids, vector<Projectile> &projectiles)
{
	player.render();

	for (int i = 0; i < asteroids.size(); i++)
	{
		Asteroid &asteroid = asteroids[i];
		asteroid.render();
	}

	for (int i = 0; i < projectiles.size(); i++)
	{
		Projectile &projectile = projectiles[i];
		projectile.render();
	}
}

int main()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
	SetTargetFPS(60);

	bool isGameRunning = true;
	vector<Asteroid> asteroids;
	vector<Projectile> projectiles;
	Timer asteroidSpawnTimer = Timer(3.0);
	Player player = Player({.x = SCREEN_WIDTH / 2.0, .y = SCREEN_HEIGHT / 2.0});

	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		if (isGameRunning)
		{
			update(isGameRunning, player, asteroids, projectiles, asteroidSpawnTimer);
		}
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		if (isGameRunning)
		{
			ClearBackground(BLACK);
			render(player, asteroids, projectiles);
		}
		else
		{
			DrawText("Game Over - Press Enter to restart", 1920 / 2 - 320, 1080 / 2 - 100, 31, RED);
		}
		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
	return 0;
}
