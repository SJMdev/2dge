#include "assetstore.hpp" 
#include "../logger/logger.hpp"
#include <SDL_image.h>
#include <cassert>

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

	for (auto [key, font] : fonts) {
		TTF_CloseFont(font);
	}

	fonts.clear();

	// TODO: clear all the fonts.
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
	assert(textures.contains(assetId));
	return textures[assetId];
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filepath, int fontSize)
{
	fonts.emplace(assetId, TTF_OpenFont(filepath.c_str(), fontSize));
}
TTF_Font* AssetStore::GetFont(const std::string& assetId) {
	return fonts[assetId];
}

