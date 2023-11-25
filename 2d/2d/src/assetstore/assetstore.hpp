#pragma once
#include <map>
#include <SDL.h>
#include <string>

class AssetStore {
private:
	std::map<std::string, SDL_Texture*> textures;
	// todo : create a map for fonts
	// todo: create a map for audio
public:
	AssetStore();
	~AssetStore();

	void ClearAssets();
	void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath);
	SDL_Texture* GetTexture(const std::string& assetId);

};