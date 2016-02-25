#include "Recurs.h"

Recurs Recurs_Create(Vector2D position, float angle)
{
	Recurs createdRecurs = {};
	createdRecurs.position = position;
	//createdRecurs.rad = 100.0f;
	return createdRecurs;
}

void Recurs_Render(Recurs * recurs, SDL_Renderer * renderer, Uint32 color)
{
	circleColor(renderer, recurs->position.x, recurs->position.y, recurs->rad, color);
}

bool Recurs_IsOnTop(Recurs * recurs, Vector2D position)
{
	Vector2D temp = recurs->position - position;
	if (temp.Length()<recurs->rad)
	{
		return true;
	}
	else
	{
		return false;
	}
}
