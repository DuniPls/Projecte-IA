#pragma once

#include "Common/GameFramework.h"

#define MAX_POINTS_IN_PATH 50


struct Recurs
{
	Vector2D position = {};
	float rad = 10.0f;
};

Recurs Recurs_Create(Vector2D position, float width, float height, float angle = 0.0f);
void Recurs_Render(Recurs* recurs, SDL_Renderer* renderer, Uint32 color);
bool Recurs_IsOnTop(Recurs* recurs, Vector2D position);

