#include "game.hpp"

// System includes
#include <iostream>
#include <fstream>
#include <sstream>

// third party includes
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>



// other includes
#include "levelloader.hpp"
#include "../logger/logger.hpp" // this is so much worse than just one folder... never use relative paths like this.
#include "../ecs/ecs.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/spritecomponent.hpp"
#include "../components/animationcomponent.hpp"
#include "../components/boxcollidercomponent.hpp"
#include "../components/keyboardcontrolledcomponent.hpp"
#include "../components/projectileemittercomponent.hpp"
#include "../components/healthcomponent.hpp"
#include "../components/textlabelcomponent.hpp"

#include "../Systems/movementsystem.hpp"
#include "../Systems/rendersystem.hpp"
#include "../Systems/animationsystem.hpp"
#include "../Systems/collisionsystem.hpp"
#include "../Systems/rendercollidersystem.hpp"
#include "../Systems/rendertextsystem.hpp"
#include "../Systems/damagesystem.hpp"
#include "../Systems/keyboardmovementsystem.hpp"
#include "../Systems/cameramovementsystem.hpp"
#include "../Systems/projectileemitsystem.hpp"
#include "../Systems/projectilelifecyclesystem.hpp"
#include "../Systems/renderhealthbarsystem.hpp"
#include "../Systems/renderguisystem.hpp"
#include "../Systems/scriptsystem.hpp"

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

	if (TTF_Init() != OK) {
		Logger::Err("error initializing SDL TTF.");
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
	

	// initialze the imgui context
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);





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
		
			
			for (const auto& element : items) {
				int tile_idx = std::stoi(element);
				auto entity = registry->CreateEntity();
				entity.Group("tiles");
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


void Game::Setup()
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
	registry->AddSystem<ProjectileLifecycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<RenderHealthBarSystem>();
	registry->AddSystem<RenderGUISystem>();
	registry->AddSystem<ScriptSystem>();


	// create the bindings between c++ and lua.

	registry->GetSystem<ScriptSystem>().CreateLuaBindings(lua);
	LevelLoader loader{};
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	
	loader.LoadLevel(lua, registry, assetStore, renderer, 2);
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
	 registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
	 registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);

	//playerPosition = playerPosition + (playerVelocity * deltaTime);

	// invoke all the systems that need to render.
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	//registry->GetSystem<DamageSystem>().Update(); // does not actually do anything.
 	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileLifecycleSystem>().Update();
	registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());

	// update the registry to process the entities that are waiting to  be created / deleted.
	registry->Update();

}

void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while( SDL_PollEvent(&sdlEvent))
	{
		// imgui SDL input.
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

		// handle core sdl events.
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
		registry->GetSystem<RenderGUISystem>().Update(registry, camera);
	}

	registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);

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
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

