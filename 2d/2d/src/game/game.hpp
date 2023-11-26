#pragma once
#include <SDL.h>


#include "../ecs/ecs.hpp"
const int FPS = 120;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;
#include "../assetstore/assetstore.hpp"

class Game
{
	

public:
	int windowWidth = 800; 
	int windowHeight = 600;
	bool fullscreen = false;

	Game();
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
	void LoadLevel(int level);
	void parseMapFile(std::string mapFileName, int tiles_per_row, int tiles_per_column);


private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	std::unique_ptr<Registry> registry = nullptr;
	std::unique_ptr<AssetStore> assetStore = nullptr;
	bool isRunning = false;
	// start of the previous frame.
	int millisecondsAtPreviousFrame = 0;
	bool frameRateCapped = false;
};


// just because I want to write it down somewhere.
// Delta time is the amount elapsed since the last frame.
// you don't want to think "how many pixels per frame, but instead: "how many pixels per SECOND."