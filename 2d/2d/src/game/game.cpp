#include "game.hpp"

// System includes
#include <iostream>
#include <fstream>
#include <sstream>

// third part inclkudes
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>

// other includes
#include "../logger/logger.hpp" // this is so much worse than just one folder... never use relative paths like this.
#include "../ecs/ecs.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/spritecomponent.hpp"
#include "../components/animationcomponent.hpp"
#include "../components/boxcollidercomponent.hpp"
#include "../components/keyboardcontrolledcomponent.hpp"
#include "../components/projectileemittercomponent.hpp"

#include "../Systems/movementsystem.hpp"
#include "../Systems/rendersystem.hpp"
#include "../Systems/animationsystem.hpp"
#include "../Systems/collisionsystem.hpp"
#include "../Systems/rendercollidersystem.hpp"
#include "../Systems/damagesystem.hpp"
#include "../Systems/keyboardmovementsystem.hpp"
#include "../Systems/cameramovementsystem.hpp"
#include "../Systems/projectileemitsystem.hpp"


#define OK 0

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;


Game::Game()
{
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
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

	windowWidth = 800;
	windowHeight = 600;

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
	
	// initialize the camera view with the entire screen area
	camera.x = 0;
	camera.y = 0;
	camera.h = windowHeight;
	camera.w = windowWidth;

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

void Game::parseMapFile(std::string mapFileName, int tiles_per_row, int tiles_per_column)
{
	glm::vec2 top_left{};
	std::vector <glm::vec2> tile_start_position(tiles_per_row * tiles_per_column);
	for (int row_idx = 0; row_idx != tiles_per_column; ++row_idx)
	{
		for (int column_idx = 0; column_idx != tiles_per_row; ++column_idx)
		{
			tile_start_position[tiles_per_row * row_idx + column_idx] = glm::vec2{ 32 * column_idx, 32 * row_idx };
		}
	}

	mapWidth = tiles_per_row * 32;
	mapHeight = tiles_per_column * 32;

	// these are our "source destinations."
	
	// how many entries do we have in the map file?
	std::ifstream file(mapFileName);

	if (file.is_open()) {
		std::string line;
		
		int y_offset = 0;
		int x_offset = 0;
		while (getline(file, line)) 
		{
			std::istringstream linestream(line);
			std::string item;
			std::vector<std::string> items;

			while (getline(linestream, item, ',')) {
				items.push_back(item);
			}

			const auto entity_idx = 0;
			const auto item_count = items.size();
			// Process items here (for example, print them)
			for (const auto& element : items) {
				int tile_idx = std::stoi(element);
				auto entity = registry->CreateEntity();
				entity.AddComponent<SpriteComponent>(
					"jungle",
					32, 32,
					0, // z_index of background tiles.
					tile_start_position[tile_idx].x, tile_start_position[tile_idx].y);
				
				entity.AddComponent<TransformComponent>(
					glm::vec2{x_offset, y_offset},
					glm::vec2{ 1.0,1.0 },
					0.0);

				x_offset += 32;
			}
			// reset x offset.
			x_offset = 0;
			
			y_offset += 32;
		}
		file.close();
	}
	else {
		std::cout << "Unable to open file." << std::endl;
	}


}

// add entities, load 
void Game::LoadLevel(int level)
{

	// add the systems that need to be processed in our game.
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<RenderColliderSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardMovementSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();


	// add assets to the asset store.
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddTexture(renderer, "jungle", "./assets/tilemaps/jungle.png");
	assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");

	
	//TODO:
	// load the tilemap
	// load the tilemap texture from ./assets/tilemaps/jungle.png
	// we need to load the file ./assets/tilemaps/jungle.map
	// tip: you can use the idea of the source rectangle.
	// tip: consider creating one entity per tile.
	// how many tiles are in this texture?
	//uint32_t format{};
	//int access{};
	//int width{};
	//int height;
	//SDL_QueryTexture(assetStore->GetTexture("jungle"),
	//	&format,
	//	&access,
	//	&width,
	//	&height);
	//int tiles_per_row = width / 32;
	//int tiles_per_column = height / 32;
	//int total_tile_count = tiles_per_row * tiles_per_column;
	//parseMapFile("./assets/tilemaps/jungle.map", tiles_per_row, tiles_per_column);

	{
		// Load the tilemap
		int tileSize = 32;
		double tileScale = 2.0;
		int mapNumCols = 25;
		int mapNumRows = 20;

		std::fstream mapFile;
		mapFile.open("./assets/tilemaps/jungle.map");

		for (int y = 0; y < mapNumRows; y++) {
			for (int x = 0; x < mapNumCols; x++) {
				char ch;
				mapFile.get(ch);
				int srcRectY = std::atoi(&ch) * tileSize;
				mapFile.get(ch);
				int srcRectX = std::atoi(&ch) * tileSize;
				mapFile.ignore();

				Entity tile = registry->CreateEntity();
				tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
				tile.AddComponent<SpriteComponent>("jungle", tileSize, tileSize, 0, false, srcRectX, srcRectY);
			}
		}
		mapFile.close();

		mapWidth = mapNumCols * tileScale * tileSize;
		mapHeight = mapNumRows * tileScale * tileSize;
	}

	
	Entity chopper = registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(00.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, // how many frames of animation?
		15,
		true);
	// distance in pixels per second for up, right, left, down.
	chopper.AddComponent<KeyboardControlledComponent>(
		glm::vec2(0, -80),
		glm::vec2(80, 0),
		glm::vec2(0, 80),
		glm::vec2(-80, 0));
	chopper.AddComponent<CameraFollowComponent>();


	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(00.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, 
		true // GUI element?
	);
	radar.AddComponent<AnimationComponent>(8, // how many frames of animation?
		5, // how many frames per second do we advance?
		true);


	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
	tank.AddComponent<BoxColliderComponent>(32, 32);
	tank.AddComponent<ProjectileEmitterComponent>(
		glm::vec2(100.0, 0.0),
		2000, // every n seconds in miliseconds
		10000,
		0,
		false);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
	truck.AddComponent<BoxColliderComponent>(32, 32);
	truck.AddComponent<ProjectileEmitterComponent>(
		glm::vec2(0.0, 100.0),
		2000, // every n seconds in miliseconds
		10000,
		0,
		false);



	// initialize game objects (why not call it that then??????????????)
	//playerPosition = glm::vec2(10.0, 20.0);
	//playerVelocity = glm::vec2(10, 50.0);
	int a = 0;
}

void Game::Setup()
{
	LoadLevel(1);
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

	// reset all event handlers for the current frame.
	 eventBus->Reset();

	 // perform the subscription of the events for all systems. (note: I think
	 // this is actually bad design: we should just do it globally instead of every frame.
	 // I am also wondering why we are 'reactive'  to events and not just add them to a queue
	 // of things to handle when the damage system gets updated. I'm not sure if we will
	 // ever get an answer.
	 registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	 registry->GetSystem<KeyboardMovementSystem>().SubscribeToEvents(eventBus);

	//playerPosition = playerPosition + (playerVelocity * deltaTime);

	// invoke all the systems that need to render.
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	registry->GetSystem<DamageSystem>().Update();
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<CameraMovementSystem>().Update(camera);

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
				}
				if (sdlEvent.key.keysym.sym == SDLK_d) {
					isDebug = !isDebug;
				}
				eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
				break;
			}

		}
	}


}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	// todo: render all game objects.

	// invoke all the systems that need to render.
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	if (isDebug) {
		registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
	}



	// with a surface:
	/*
	{
		SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}*/

	// draw a PNG texture.
	//SDL_Texture* texture = IMG_LoadTexture(renderer, "assets/images/tank-tiger-right.png");
	//SDL_Point size{};
	//SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);
	//
	//// What is the destination rectangle that we want to place our texture? @x,y 10,10 with texture width and texture height.
	//SDL_Rect destinationRect = { 
	//	static_cast<int>(playerPosition.x),
	//	static_cast<int>(playerPosition.y),
	//	size.x,
	//	size.y };
	//// the third argument is the "source rectangle", but what that means is if you want to copy a section of the texture instead of the whole thing.
	//SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
	//SDL_DestroyTexture(texture);

	// swap buffers
	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

