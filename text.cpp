#include "text.h"

Text::Text(string content, Vector2 position, float fontSize, Color color)
{
  this->content = content;
  this->position = position;
  this->fontSize = fontSize;
  this->color = color;
};

void Text::render()
{
  DrawText(
      content.c_str(),
      position.x,
      position.y,
      fontSize,
      color);
}