#ifndef TEXT
#define TEXT

#include "raylib.h"
#include <string>

using namespace std;

class Text
{
public:
    string content;
    Vector2 position;
    float fontSize;
    Color color;

    Text(string content, Vector2 position, float fontSize, Color color);
    void render();
};

#endif