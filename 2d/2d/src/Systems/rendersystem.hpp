#pragma once
#include "../ecs/ecs.hpp"
#include "../components/spritecomponent.hpp"
#include "../components/transformcomponent.hpp"
#include <SDL.h>
#include "../assetstore/assetstore.hpp"
#include <algorithm>

class RenderSystem : public System {
public:

	RenderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera) {
		// sort all the entities of our system by zIndex.

		std::vector<Entity> renderableEntities = GetSystemEntities();
		std::sort(renderableEntities.begin(), renderableEntities.end(), [](const Entity& lhs, const Entity& rhs) {
			return lhs.GetComponent<SpriteComponent>().zIndex < rhs.GetComponent<SpriteComponent>().zIndex;
			});


		for (auto& entity : renderableEntities) {
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto sprite = entity.GetComponent<SpriteComponent>();

			// set the source rectangle of our original sprite texture (select from texture atlas).
			SDL_Rect srcRect = sprite.srcRect;

			// do not update the position relative to the camera if the sprite is fixed.
			SDL_Rect dstRect{
			.x = static_cast<int>(transform.position.x - (sprite.isFixed ?  0 :camera.x)),
			.y = static_cast<int>(transform.position.y - (sprite.isFixed ? 0  : camera.y)),
			.w = static_cast<int>(sprite.width * transform.scale.x),
			.h = static_cast<int>(sprite.height * transform.scale.y)
			};

			// huh, what happens if dstRect is out of  range of the renderer's?
			SDL_RenderCopyEx(renderer,
				assetStore->GetTexture(sprite.assetId),
				&srcRect,
				&dstRect,
				transform.rotation,
				nullptr, // rotate around the center point.
				SDL_FLIP_NONE
			);

		}


	}
};