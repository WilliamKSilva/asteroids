#ifndef TEXT
#define TEXT

#include "raylib.h"
#include <string>

using namespace std;

class Text
{
public:
    Text(string content, Vector2 position, float fontSize, Color color) {
        this->content = content;
        this->position = position;
        this->fontSize = fontSize;
        this->color = color;
    };

    string content;
    Vector2 position;
    float fontSize;
    Color color;

    void render()
    {
        DrawText(
            content.c_str(),
            position.x,
            position.y,
            fontSize,
            color);
    }
};

#endif