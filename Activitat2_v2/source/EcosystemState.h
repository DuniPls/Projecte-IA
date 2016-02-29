/* ========================================================================
   File: EcosystemState.h
   Revision: 0.1
   Creator: David Collado Ochoa
   Notice: (C) Copyright 2016 by David Collado Ochoa. All Rights Reserved.
   ======================================================================== */

#pragma once
#define BOID_POOL_SIZE 30

#include "Common/GameFramework.h"
#include "Boid.h"
#include "Obstacle.h"
#include "Recurs.h"

class EcosystemState : public BaseState
{
public:
	virtual void Init() override;
	virtual void Deinit() override;
	virtual StateCode HandleInput() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
	void CreateBoid(int x, int y);
	void ChangeBoidsBehavior(Behaviour newBehavior);

	// State Data

	// Graphics
	Texture boidTexture;

	// Entities
	Boid boidPool[BOID_POOL_SIZE];
	int boidPoolOccupation = 0;
	Boid boid;
	Recurs recurs[15];

	// Trail
	Trail trailPool[BOID_POOL_SIZE];
	int trailPoolOccupation = 0;
	Trail boidTrail;
	float K_TRAIL_UPDATE_THRESHOLD;
	float trailUpdateCounter;

	// Fonts and Debug
	FC_Font* fontVerySmall;
	FC_Font* fontSmall;
	FC_Font* fontBig;
	char* title;
	char* theoricUpdateInformation;
	char* theroicAlgorithmInformation;
	bool showDebug = false;

	// Pause Params
	float timeScale = 1.0f;
	bool isPaused = false;
	bool runOneFrame = false;

};