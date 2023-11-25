#include "assetstore.hpp" 
#include "../logger/logger.hpp"
#include <SDL_image.h>

AssetStore::AssetStore() {
	Logger::Log("AssetStore Constructor called!");
}

AssetStore::~AssetStore() {
}


void AssetStore::ClearAssets() {
	for (auto& [key, texture] : textures)
	{
		SDL_DestroyTexture(texture);
	}

	textures.clear();
}


void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	textures.emplace(assetId, texture);

	Logger::Log("New texture added to the asset store with id = " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId)
{
	return textures[assetId];
}
