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
    void LoadLevel(
        sol::state& lua,
        const std::unique_ptr<Registry>& registry,
        const std::unique_ptr<AssetStore>& assetStore,
        SDL_Renderer* renderer,
        int levelNumber);
};

