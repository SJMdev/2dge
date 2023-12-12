#pragma once

#include "../ecs/ecs.hpp"
#include <memory>
#include <SDL.h>
#include "../assetstore/assetstore.hpp"
#include <sol/sol.hpp>

class LevelLoader {


public:
	LevelLoader() = default;
	~LevelLoader() = default;
	void loadLevel(
		sol::state& lua,
		std::unique_ptr<Registry>& registry,
		std::unique_ptr<AssetStore>& assetStore,
		SDL_Renderer* renderer,
		int levelNumber);
};
