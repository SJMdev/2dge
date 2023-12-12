#pragma once
#include <SDL.h>


#include "../ecs/ecs.hpp"
#include "../assetstore/assetstore.hpp"
#include "../eventbus/eventbus.hpp"
#include <sol/sol.hpp>
const int FPS = 120;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game
{
	

public:
	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;

	bool fullscreen = false;
	bool isDebug = false;

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
	void parseMapFile(std::string mapFileName, int tiles_per_row, int tiles_per_column);


private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	sol::state lua;

	std::unique_ptr<Registry> registry = nullptr;
	std::unique_ptr<AssetStore> assetStore = nullptr;
	std::unique_ptr<EventBus> eventBus = nullptr;
	SDL_Rect camera;
	bool isRunning = false;
	// start of the previous frame.
	int millisecondsAtPreviousFrame = 0;
	bool frameRateCapped = false;
};


// just because I want to write it down somewhere.
// Delta time is the amount elapsed since the last frame.
// you don't want to think "how many pixels per frame, but instead: "how many pixels per SECOND."