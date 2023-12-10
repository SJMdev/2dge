#pragma once
#include "../ecs/ecs.hpp"
#include <string>


struct SpriteComponent
{
	std::string assetId;
	int width;
	int height;
	int zIndex;
	SDL_RendererFlip flip;
	bool isFixed;
	SDL_Rect srcRect; // where are we in the atlas?

	SpriteComponent(
		std::string assetId = "",
		int width = 0,
		int height = 0,
		int zIndex = 0,
		bool isFixed = false,
		int srcRectX = 0,
		int srcRectY = 0)
		:
		assetId(assetId),
		width(width),
		height(height),
		zIndex(zIndex),
		flip(SDL_FLIP_NONE),
		isFixed(isFixed),
		srcRect{ srcRectX, srcRectY, width, height }
	{}


};