#pragma once
#include <SDL.h>


class Game
{
	

public:
	int windowWidth = 800; 
	int windowHeight = 600;
	bool fullscreen = false;

	Game() = default;
	~Game() = default;

	Game(Game&& game) = delete;
	Game(Game& game) = delete;

	void Initialize();
	void Run();
	void Setup();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	bool isRunning = false;
	
};