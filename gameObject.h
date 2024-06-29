#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "raylib.h"

class Circle
{
public:
	int centerX;
	int centerY;
	float radius;
	Color color;
};

class TexturePro
{
public:
	Rectangle sourceRec;
	Rectangle destRec;
	Vector2 origin;
	float rotation;
};

class GameObject
{
public:
	Vector2 position;
	Texture2D sprite;
	TexturePro texturePro;

	GameObject(Vector2 position, const char *spritePath);
	void render();
	void updatePosition(Vector2 position);
	bool isOutOfBounds();
	bool checkCollision(Rectangle gameObject);
};
#endif