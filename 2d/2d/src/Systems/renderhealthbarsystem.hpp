#pragma once
#include "../ecs/ecs.hpp"
#include "../components/healthcomponent.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/spritecomponent.hpp"


class RenderHealthBarSystem : public System {
public:
	RenderHealthBarSystem() {
		RequireComponent<HealthComponent>();
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
		for (auto& entity : GetSystemEntities()) {
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();
			const auto health = entity.GetComponent<HealthComponent>();

			SDL_Color healthBarColor = { 255, 255 ,255 };

			if (health.healthPercentage >= 0 && health.healthPercentage < 25) {
				// 0-25 = red
				healthBarColor = { 255, 0, 0 };
			}

			if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
				// 25-75 = yellow
				healthBarColor = { 255, 255, 0 };
			}

			if (health.healthPercentage >= 75 && health.healthPercentage <= 100) {
				// 75-100 = green
				healthBarColor = { 0, 255, 0 };
			}

			// position the leahtbar indicator in the middle-bottom part of the entity sprite
			int healthBarWidth = 15;
			int healthBarHeight = 3;
			double healthBarPosX = (transform.position.x + (sprite.width * transform.scale.x)) - camera.x;
			double healthBarPosY = (transform.position.y) - camera.y;

			SDL_Rect healthBarRectangle = {
				.x = static_cast<int>(healthBarPosX),
				.y = static_cast<int>(healthBarPosY),
				.w = static_cast<int>(healthBarWidth * (health.healthPercentage / 100.0)),
				.h = static_cast<int>(healthBarHeight)
			};

			SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
			SDL_RenderFillRect(renderer, &healthBarRectangle);

			// render the health percentage text label indicator.
			std::string healthText = std::to_string(health.healthPercentage);
			auto* font = assetStore->GetFont("pico8-font-5");
			SDL_Surface* surface = TTF_RenderText_Blended(font, healthText.c_str(), healthBarColor);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;
			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
			SDL_Rect healthBarTextRectangle = {
				.x = static_cast<int>(healthBarPosX),
				.y = static_cast<int>(healthBarPosY) + 5,
				.w = labelWidth,
				.h = labelHeight
			};

			SDL_RenderCopy(renderer, texture, NULL, &healthBarTextRectangle);
			

		}
	}


};