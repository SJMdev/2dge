#include "levelloader.hpp"

#include "../logger/logger.hpp"

#include <fstream>
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/spritecomponent.hpp"
#include "../components/animationcomponent.hpp"
#include "../components/boxcollidercomponent.hpp"
#include "../components/keyboardcontrolledcomponent.hpp"
#include "../components/projectileemittercomponent.hpp"
#include "../components/healthcomponent.hpp"
#include "../components/textlabelcomponent.hpp"
#include "../components/camerafollowcomponent.hpp"
#include "game.hpp" // scary... almost circular include since in game.cpp we include levelloader.hpp.

#include <sol/sol.hpp>

void LevelLoader::loadLevel(
	sol::state& lua,
	std::unique_ptr<Registry>& registry,
	std::unique_ptr<AssetStore>& assetStore,
	SDL_Renderer* renderer,
	int levelNumber) {

	lua.open_libraries(sol::lib::base);

	// load the entiteis and components from ./assets/scripts/level1.lua
	// note: THIS DOES NOT EXECUTE THE SCRIPT!
	sol::load_result script = lua.load_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");

	if (!script.valid()) {
		sol::error err = script;
		std::string errorMessage = err.what();
		Logger::Err("Error loading lua script: " + errorMessage);
	}

	// executes the script using the sol state.
	lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");

	// read the big table for the current level.
	//is this a copy?
	sol::table level = lua["level"];


	/////////////////////////////////////////////////////////////////////////
	// read the level assets
	////////////////////////////////////////////////////////////////////////
	
	sol::table assets = level["assets"];

	int i = 0;
	while (true) {
		sol::optional<sol::table> hasasset = assets[i];
		
		if (hasasset == sol::nullopt) {
			break;
		}
		sol::table asset = assets[i];
		std::string assetType = asset["type"];
		if (assetType == "texture") {
			std::string assetId = asset["id"];
			assetStore->AddTexture(renderer, assetId, asset["file"]);
			Logger::Log("New texture added to assetstore, id: " + assetId);
		}

		if (assetType == "font") {
			std::string assetId = asset["id"];
			assetStore->addFont(assetId, asset["file"], asset["font_size"]);
			Logger::Log("New Font added to assetstore, id: " + assetId);

		}
		// loop over all entries of the assets table
		i++;
	}



	/////////////////////////////////////////////////////////////////////////
	// read the level tilemap
	////////////////////////////////////////////////////////////////////////
	// TODO:



	/////////////////////////////////////////////////////////////////////////
	// read the level entites and components
	////////////////////////////////////////////////////////////////////////
	// TODO:



	//// add assets to the asset store.
	//assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	//assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	//assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	//assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	//assetStore->AddTexture(renderer, "jungle", "./assets/tilemaps/jungle.png");
	//assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
	//assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");

	//assetStore->addFont("charriot-font", "./assets/fonts/charriot.ttf", 20);
	//assetStore->addFont("pico8-font-5", "./assets/fonts/charriot.ttf", 5);
	//assetStore->addFont("pico8-font-10", "./assets/fonts/charriot.ttf", 10);

	//{
	//	// Load the tilemap
	//	int tileSize = 32;
	//	double tileScale = 2.0;
	//	int mapNumCols = 25;
	//	int mapNumRows = 20;

	//	std::fstream mapFile;
	//	mapFile.open("./assets/tilemaps/jungle.map");

	//	for (int y = 0; y < mapNumRows; y++) {
	//		for (int x = 0; x < mapNumCols; x++) {
	//			char ch;
	//			mapFile.get(ch);
	//			int srcRectY = std::atoi(&ch) * tileSize;
	//			mapFile.get(ch);
	//			int srcRectX = std::atoi(&ch) * tileSize;
	//			mapFile.ignore();

	//			Entity tile = registry->CreateEntity();
	//			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
	//			tile.AddComponent<SpriteComponent>("jungle", tileSize, tileSize, 0, false, srcRectX, srcRectY);
	//		}
	//	}
	//	mapFile.close();

	//	Game::mapWidth = mapNumCols * tileScale * tileSize;
	//	Game::mapHeight = mapNumRows * tileScale * tileSize;
	//}


	//Entity chopper = registry->CreateEntity();
	//chopper.Tag("player");
	//chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	//chopper.AddComponent<RigidBodyComponent>(glm::vec2(00.0, 0.0));
	//chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	//chopper.AddComponent<AnimationComponent>(2, // how many frames of animation?
	//	15,
	//	true);
	//// distance in pixels per second for up, right, left, down.
	//chopper.AddComponent<KeyboardControlledComponent>(
	//	glm::vec2(0, -80),
	//	glm::vec2(80, 0),
	//	glm::vec2(0, 80),
	//	glm::vec2(-80, 0));
	//chopper.AddComponent<CameraFollowComponent>();
	//chopper.AddComponent<HealthComponent>(100);
	//chopper.AddComponent<BoxColliderComponent>(32, 32);
	//chopper.AddComponent<ProjectileEmitterComponent>(
	//	glm::vec2(150.0, 150.0),
	//	0,
	//	10000,
	//	10, // percentage hit damage
	//	true);

	//Entity radar = registry->CreateEntity();
	//radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 74, 10), glm::vec2(1.0, 1.0), 0.0);
	//radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	//radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2,
	//	true // GUI element?
	//);
	//radar.AddComponent<AnimationComponent>(8, // how many frames of animation?
	//	5, // how many frames per second do we advance?
	//	true);


	//Entity tank = registry->CreateEntity();
	//tank.Group("enemies");
	//tank.AddComponent<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(1.0, 1.0), 0.0);
	//tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
	//tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
	//tank.AddComponent<BoxColliderComponent>(32, 32);
	////tank.AddComponent<ProjectileEmitterComponent>(
	////	glm::vec2(100.0, 0.0),
	////	5000, // every n seconds in miliseconds
	////	3000,
	////	10, // percent hit damage
	////	false);
	//tank.AddComponent<HealthComponent>(100);

	//Entity treeA = registry->CreateEntity();
	//treeA.Group("obstacles");
	//treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 495), glm::vec2(1.0, 1.0), 0.0);
	//treeA.AddComponent<RigidBodyComponent>(glm::vec2(00.0, 0.0));
	//treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
	//treeA.AddComponent<BoxColliderComponent>(16, 32);


	//Entity treeB = registry->CreateEntity();
	//treeB.Group("obstacles");
	//treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 495), glm::vec2(1.0, 1.0), 0.0);
	//treeB.AddComponent<RigidBodyComponent>(glm::vec2(00.0, 0.0));
	//treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
	//treeB.AddComponent<BoxColliderComponent>(16, 32);


	//Entity truck = registry->CreateEntity();
	//truck.Group("enemies");
	//truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	//truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	//truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
	//truck.AddComponent<BoxColliderComponent>(32, 32);
	//truck.AddComponent<ProjectileEmitterComponent>(
	//	glm::vec2(0.0, 100.0),
	//	2000, // every n seconds in miliseconds
	//	5000,
	//	50, // percent hit damage.
	//	false);
	//truck.AddComponent<HealthComponent>(100);


	//Entity label = registry->CreateEntity();
	//label.Group("labels");
	//label.AddComponent<TextLabelComponent>(
	//	glm::vec2(Game::windowWidth / 2 - 40, 10),
	//	"CHOPPER 1.0",
	//	"charriot-font",
	//	SDL_Color{ .r = 0, .g = 255, .b = 0, .a = 255 },
	//	true
	//);



	//// initialize game objects (why not call it that then??????????????)
	////playerPosition = glm::vec2(10.0, 20.0);
	////playerVelocity = glm::vec2(10, 50.0);
	//int a = 0;
}