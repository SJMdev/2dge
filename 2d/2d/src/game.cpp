#include "game.hpp"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

#define OK 0

namespace
{
	glm::vec2 playerPosition{};
	glm::vec2 playerVelocity{};
}

void Game::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != OK) {
		std::cerr << "error initializing SDL." << '\n';
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
		std::cerr << "Error creating SDL window." << '\n';
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		std::cerr << "Error creating SDL renderer" << '\n';
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
	// initialize game objects (why not call it that then??????????????)
	playerPosition = glm::vec2(10.0, 20.0);
	playerVelocity = glm::vec2(0.05, 0.0);

}


void Game::Update()
{
	// TODO: update game objects.
	playerPosition = playerPosition + playerVelocity;
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

