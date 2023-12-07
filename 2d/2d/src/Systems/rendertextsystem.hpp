#pragma once
#include "../components/textlabelcomponent.hpp"
#include "../ecs/ecs.hpp" 
#include "../assetstore/assetstore.hpp"

class RenderTextSystem : public System {

public:
	RenderTextSystem() {
		RequireComponent<TextLabelComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
		for (auto entity : GetSystemEntities()) {
			auto& textlabelcomponent = entity.GetComponent<TextLabelComponent>();

			auto* font = assetStore->GetFont(textlabelcomponent.assetId);

			// generate a surface that will contain our text.
			SDL_Surface* surface = TTF_RenderText_Blended(
				font,
				textlabelcomponent.text.c_str(),
				textlabelcomponent.color);


			// create a texture that contains our text
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;

			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

			SDL_Rect dstRect = {
				.x = static_cast<int>(textlabelcomponent.position.x - (textlabelcomponent.isFixed ? 0 : camera.x)),
				.y = static_cast<int>(textlabelcomponent.position.y - (textlabelcomponent.isFixed ? 0 : camera.y)),
				.w = labelWidth,
				.h = labelHeight
			};

			SDL_RenderCopy(renderer, texture, NULL, &dstRect);

		}
	}
};