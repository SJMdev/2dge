#include "game.hpp"

// System includes
#include <iostream>


// third part inclkudes
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

// other includes
#include "../logger/logger.hpp" // this is so much worse than just one folder... never use relative paths like this.
#include "../ecs/ecs.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../Systems/movementsystem.hpp"

#define OK 0

Game::Game()
{
	registry = std::make_unique<Registry>();
}

namespace
{
	glm::vec2 playerPosition{};
	glm::vec2 playerVelocity{};
}

void Game::Initialize()
{
	Logger::Log("hello!~\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) != OK) {
		Logger::Err("error initializing SDL");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	if (fullscreen)
	{
		windowWidth = displayMode.w;
		windowHeight = displayMode.h;
	}
	

	uint32_t window_flags = SDL_WINDOW_BORDERLESS;
	window = SDL_CreateWindow(
		nullptr,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		window_flags
	);

	if (!window)
	{
		Logger::Err("Error creating SDL window.");
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		Logger::Err("Error creating SDL renderer.");
		return;
	}

	if (fullscreen)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	

	isRunning = true;
}

void Game::Run()
{
	Setup();
	while (isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}


void Game::Setup()
{
	// add the systems that need to be processed in our game.
	registry->AddSystem<MovementSystem>();

	Entity tank = registry->CreateEntity();
	registry->AddComponent<TransformComponent>(tank, glm::vec2(10.0, 30.0), glm::vec2(1.0,1.0), 0.0);
	registry->AddComponent<RigidBodyComponent>(tank, glm::vec2(10.0, 30.0));


	// initialize game objects (why not call it that then??????????????)
	playerPosition = glm::vec2(10.0, 20.0);
	playerVelocity = glm::vec2(10, 5.0);

}

// fixing (not repairing, but "keep steady") the timestep: sleep until we hit the frame timer.
void Game::Update()
{
	//@NOTE(SJM): busy waiting solution.
	{
	// SDL_getTicks gets time elapsed since sdl loaded.
	// if we are too fast, waste some time until we reach milisecs_per_frame (for stable physics).
	// how much time has passed since the previous frame?
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), millisecondsAtPreviousFrame + MILLISECONDS_PER_FRAME));
	}

	//@NOTE(SJM): SDL_Delay solution (suspend thread?)
	if (frameRateCapped)
	{
		int TimeElapsedSinceLastFrame = (SDL_GetTicks() - millisecondsAtPreviousFrame);
		int timeToWait = MILLISECONDS_PER_FRAME - TimeElapsedSinceLastFrame;
		if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME)
		{
			SDL_Delay(timeToWait);
		}
	}
	
	// store the start of "this" (the upcoming) frame.
	float deltaTime = (SDL_GetTicks() - millisecondsAtPreviousFrame) / 1000.0f;
	 millisecondsAtPreviousFrame = SDL_GetTicks();
	// TODO: update game objects.
	playerPosition = playerPosition + (playerVelocity * deltaTime);

	// ask all the systems to update
	registry->GetSystem<MovementSystem>().Update();
	//TODO: registry->getSystem<CollisionSystem>().Update();
	// 
	// update the registry to process the entities that are waiting to  be created / deleted.
	registry->Update();

}

void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while( SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
			case SDL_QUIT:
			{
				isRunning = false;
				break;
			}
			case SDL_KEYDOWN:
			{
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
				{
					isRunning = false;
					break;
				}
			}
		}
	}


}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	// todo: render all game objects.

	// with a surface:
	/*
	{
		SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}*/

	// draw a PNG texture.
	SDL_Texture* texture = IMG_LoadTexture(renderer, "assets/images/tank-tiger-right.png");
	SDL_Point size{};
	SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);
	
	// What is the destination rectangle that we want to place our texture? @x,y 10,10 with texture width and texture height.
	SDL_Rect destinationRect = { 
		static_cast<int>(playerPosition.x),
		static_cast<int>(playerPosition.y),
		size.x,
		size.y };
	// the third argument is the "source rectangle", but what that means is if you want to copy a section of the texture instead of the whole thing.
	SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
	SDL_DestroyTexture(texture);

	// swap buffers
	SDL_RenderPresent(renderer);
}



void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

