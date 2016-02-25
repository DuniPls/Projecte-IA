/* ========================================================================
   File: EcosystemState.cpp
   Revision: 0.1
   Creator: David Collado Ochoa
   Notice: (C) Copyright 2016 by David Collado Ochoa. All Rights Reserved.
   ======================================================================== */

#include "EcosystemState.h"

void EcosystemState::Init()
{
	// Initialize Fonts of different sizes
	SetResourcesFilePath("Fonts/nokiafc22.ttf");
	fontVerySmall = FC_CreateFont();
	FC_LoadFont(fontVerySmall, display->renderer, resourcesFilePath, 12, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
	fontSmall = FC_CreateFont();
	FC_LoadFont(fontSmall, display->renderer, resourcesFilePath, 18, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
	fontBig = FC_CreateFont();
	FC_LoadFont(fontBig, display->renderer, resourcesFilePath, 26, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);

	// Initialize Textures
	boidTexture = {};
	SetResourcesFilePath("Images/Boid.png");
	if (Texture_LoadFromFile(&boidTexture, display->renderer, resourcesFilePath) == false)
	{
		SDL_Log("Failed to load Image!\n");
	}

	playerTexture = {};
	SetResourcesFilePath("Images/Player.png");
	if (Texture_LoadFromFile(&playerTexture, display->renderer, resourcesFilePath) == false)
	{
		SDL_Log("Failed to load Image!\n");
	}

	// Initialize Entities
	//player = {};
	player.position = { FIXED_WIDTH / 2,  FIXED_HEIGHT / 2 };
	player.width = playerTexture.width;
	player.height = playerTexture.height;

	// Seed Random
	srand(5555);
	int randomX = IntegerUtils::RandomRange(0, FIXED_WIDTH);
	int randomY = IntegerUtils::RandomRange(0, FIXED_HEIGHT);

	// Boids
	//boid = {};
	boid.position = { (float)randomX, (float)randomY };
	boid.width = boidTexture.width;
	boid.height = boidTexture.height;

	boid.target = &player;
	boid.SetBehaviour(Behaviour::WANDER);
	boid.InitDebug(display->renderer, fontVerySmall);
	for (size_t i = 0; i < 10; i++)
	{
		srand(rand() % 10000);
		int randomX = IntegerUtils::RandomRange(0, FIXED_WIDTH);
		int randomY = IntegerUtils::RandomRange(0, FIXED_HEIGHT);
		recurs[i].position = { (float)randomX, (float)randomY };
	}
	

	// Initialize Text
	title = (char*)"SEEK Kinematic Algorithm";
	theoricUpdateInformation = (char*)"Position = Position + Velocity * deltaTime";
	theroicAlgorithmInformation = (char*)"Desired Vel. = Normalized(Target.Position - Position) * Max_Speed\nVelocity = Desired Vel.";

	// Initialize Juice
	//boidTrail = {};
	Trail_Initialize(&boidTrail, 80, 4, 4);
	trailUpdateCounter = 0;
	K_TRAIL_UPDATE_THRESHOLD = 0.0225f;
}

void EcosystemState::Deinit()
{
	Trail_Free(&boidTrail);
	boid.DeinitDebug();
	Texture_Free(&boidTexture);
	Texture_Free(&playerTexture);
	FC_FreeFont(fontVerySmall);
	FC_FreeFont(fontSmall);
	FC_FreeFont(fontBig);
}

StateCode EcosystemState::HandleInput()
{
	if (InputSingleton::Instance()->doQuit)
	{
		return StateCode::QUIT;
	}
	// keyboard Input Handling
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		return StateCode::MENU;
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_SPACE))
	{
		paused = !paused;
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_Z))
	{
		timeScale = 1.0f;
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_X))
	{
		timeScale = 0.75f;
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_C))
	{
		timeScale = 0.5f;
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_V))
	{
		timeScale = 0.25f;
	}
#if DEBUG
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_D))
	{
		showDebug = !showDebug;
	}
#endif
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_N))
	{
		if (paused == false)
		{
			paused = true;
		}
		else
		{
			runOneFrame = true;
		}
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_R))
	{
		boidPoolOccupation = 0;
		trailPoolOccupation = 0;
		boid.position = { (float)(FIXED_WIDTH / 2), (float)(FIXED_HEIGHT / 2) };
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_1))
	{
		ChangeBoidsBehavior(Behaviour::SEEK_KINEMATIC);
		title = (char*)"SEEK Kinematic Algorithm";
		theoricUpdateInformation = (char*)"Position = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"Desired Vel. = Normalized(Target.Position - Position) * Max_Speed\nVelocity = Desired Vel.";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_2))
	{
		ChangeBoidsBehavior(Behaviour::SEEK_STEERING);
		title = (char*)"SEEK Steering Behaviour";
		theoricUpdateInformation = (char*)"Accel = Steering Force / Mass\nVelocity = Velocity + Accel * deltaTime\nPosition = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"Desired Vel. = Normalized(Target.Position - Position) * Max_Speed\nSteer. Force = (Desired Vel. - Velocity) / Max_Speed\nSteer. Force *= Max_Force";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_3))
	{
		ChangeBoidsBehavior(Behaviour::FLEE_KINEMATIC);
		title = (char*)"FLEE Kinematic Behaviour";
		theoricUpdateInformation = (char*)"Position = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"Desired Vel. = Normalized(Position - Target.Position) * Max_Speed\nVelocity = Desired Vel.";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_4))
	{
		ChangeBoidsBehavior(Behaviour::FLEE_STEERING);
		title = (char*)"FLEE Steering Behaviour";
		theoricUpdateInformation = (char*)"Accel = Steering Force / Mass\nVelocity = Velocity + Accel * deltaTime\nPosition = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"Desired Vel. = Normalized(Position - Target.Position) * Max_Speed\nSteer. Force = (Desired Vel. - Velocity) / Max_Speed\nSteer. Force *= Max_Force";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_5))
	{
		ChangeBoidsBehavior(Behaviour::ARRIVE);
		title = (char*)"ARRIVE Steering Behaviour";
		theoricUpdateInformation = (char*)"Accel = Steering Force / Mass\nVelocity = Velocity + Accel * deltaTime\nPosition = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"if (Distance to Target > Slowing Radius) then SEEK at Max_Speed\nelse SEEK at Max_Speed * Factor\nFactor = Distance to Target / Slowing Radius";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_6))
	{
		ChangeBoidsBehavior(Behaviour::PURSUE);
		title = (char*)"PURSUE Steering Behaviour";
		theoricUpdateInformation = (char*)"Accel = Steering Force / Mass\nVelocity = Velocity + Accel * deltaTime\nPosition = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"SEEK with PREDICTED Target Position\nPREDICTED Tar. Pos. = Curr. Tar. Pos. + Curr. Tar. Vel. * T\nT = Distance To Target / Velocity";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_7))
	{
		ChangeBoidsBehavior(Behaviour::EVADE);
		title = (char*)"EVADE Steering Behaviour";
		theoricUpdateInformation = (char*)"Accel = Steering Force / Mass\nVelocity = Velocity + Accel * deltaTime\nPosition = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"FLEE with PREDICTED Target Position\nPREDICTED Tar. Pos. = Curr. Tar. Pos. + Curr. Tar. Vel. * T\nT = Distance To Target / Velocity";
	}
	else if (InputSingleton::Instance()->IsKeyDown(SDL_SCANCODE_8))
	{
		ChangeBoidsBehavior(Behaviour::WANDER);
		title = (char*)"WANDER Steering Behaviour";
		theoricUpdateInformation = (char*)"Accel = Steering Force / Mass\nVelocity = Velocity + Accel * deltaTime\nPosition = Position + Velocity * deltaTime";
		theroicAlgorithmInformation = (char*)"Update Wander Angle\nUse Parametric Equation for the Circle to get Target Position\nT.x = Pos.x + W_Offset.x * Vel.normalized().x + W_Radius * cos(W_Angle)\nT.y = Pos.y + W_Offset.y * Vel.normalized().y + W_Radius * sin(W_Angle)\nSEEK Target\n";
	}

	// Mouse Input Handling
	if (InputSingleton::Instance()->mouseButtonState)
	{
		SDL_Log("Create Boid!");
		CreateBoid(InputSingleton::Instance()->mousePosition.x,
			InputSingleton::Instance()->mousePosition.y);
	}

	// Gamepad Input Handling
	if (InputSingleton::Instance()->gamepadButtonsState.start.isDown
		&& !InputSingleton::Instance()->gamepadButtonsState.start.wasDown)
	{
		paused = !paused;
	}

	player.HandleInput();

	return StateCode::CONTINUE;
}

void EcosystemState::Update(float deltaTime)
{
	// Pause management
	if (paused == true && runOneFrame == false)
	{
		return;
	}
	runOneFrame = false;

	// Calculate time
	float currentDeltaTime = deltaTime * timeScale;

	// Update Entities
	player.Update(currentDeltaTime);
	boid.Update(currentDeltaTime);

	// Update Trail
	bool updateTrail = false;
	trailUpdateCounter += currentDeltaTime;
	if (trailUpdateCounter > K_TRAIL_UPDATE_THRESHOLD)
	{
		Trail_Update(&boidTrail, boid.position.x, boid.position.y);
		trailUpdateCounter = 0.0f;
		updateTrail = true;
	}

	// Update Boids in Pool
	for (int i = 0; i < boidPoolOccupation; i++)
	{
		boidPool[i].Update(currentDeltaTime);
		if (updateTrail == true)
		{
			Trail_Update(&trailPool[i], boidPool[i].position.x, boidPool[i].position.y);
		}
	}
}

void EcosystemState::Render()
{
	// Render Text
	FC_DrawAlign(fontBig, display->renderer,
		0,
		FIXED_HEIGHT - 160,
		FC_ALIGN_LEFT,
		title);

	FC_DrawAlign(fontSmall, display->renderer,
		FIXED_WIDTH,
		0,
		FC_ALIGN_RIGHT,
		theoricUpdateInformation);
	FC_DrawAlign(fontSmall, display->renderer,
		0,
		FIXED_HEIGHT - 120,
		FC_ALIGN_LEFT,
		theroicAlgorithmInformation);

	if (isPaused == true)
	{
		FC_DrawAlign(fontSmall, display->renderer,
			0,
			0,
			FC_ALIGN_LEFT,
			(char*)"Pause");
	}
	else
	{
		FC_DrawAlign(fontSmall, display->renderer,
			0,
			0,
			FC_ALIGN_LEFT,
			(char*)"TimeScale = %.2fx", timeScale);
	}

	// Render Trail
	Trail_Render(&boidTrail, display->renderer);

	// Render Trails in Pool
	for (int i = 0; i < trailPoolOccupation; i++)
	{
		Trail_Render(&trailPool[i], display->renderer);
	}

	// Render Boids in Pool
	for (int i = 0; i < boidPoolOccupation; i++)
	{
		boidPool[i].Render(&boidTexture, display->renderer);
	}

	// Render Entities
	player.Render(&playerTexture, display->renderer);
	boid.Render(&boidTexture, display->renderer);
	for (size_t i = 0; i < 10; i++)		
		Recurs_Render(&recurs[i], display->renderer, Colors::ALIZARIN);


	// Render Debug 
	if (showDebug)
	{
		boid.DrawDebug(display->renderer);
	}
}

void EcosystemState::CreateBoid(int x, int y)
{
	if (boidPoolOccupation < BOID_POOL_SIZE)
	{
		//boidPool[boidPoolOccupation] = {};
		boidPool[boidPoolOccupation].position = { (float)x, (float)y };
		boidPool[boidPoolOccupation].width = boidTexture.width;
		boidPool[boidPoolOccupation].height = boidTexture.height;

		boidPool[boidPoolOccupation].target = &player;
		boidPool[boidPoolOccupation].SetBehaviour(boid.currentBehaviour);
		++boidPoolOccupation;

		trailPool[trailPoolOccupation] = {};
		Trail_Initialize(&trailPool[trailPoolOccupation], 80, 4, 4);
		++trailPoolOccupation;
	}
	else
	{
		SDL_Log("Cannot create Boid. Pool is Full!");
	}
}

void EcosystemState::ChangeBoidsBehavior(Behaviour newBehavior)
{
	boid.SetBehaviour(newBehavior);
	for (int i = 0; i < boidPoolOccupation; i++)
	{
		boidPool[i].SetBehaviour(newBehavior);
	}
}
