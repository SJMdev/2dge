#pragma once
#include "../ecs/ecs.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/projectileemittercomponent.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/boxcollidercomponent.hpp"
class ProjectileEmitSystem : public System {
public:
	ProjectileEmitSystem()
	{
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(std::unique_ptr<Registry>& registry) {
		// actually emit a projectile ?
		for (auto entity : GetSystemEntities()) {
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto transform  = entity.GetComponent<TransformComponent>();

			// check if it is time to emit a new projectile
			if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency) {
				glm::vec2 projectilePosition = transform.position;
				if (entity.HasComponent<SpriteComponent>())
				{
					// I do not like hwo scale is used throughout instead of multiplying the dimensions once.
					// it seems like things can go wrong very fast.
					const auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePosition.x += (transform.scale.x * sprite.width / 2);
					projectilePosition.y += (transform.scale.y * sprite.height / 2);
				}
				// todo: add a new projectile entity to the registry.
				Entity projectile = registry->CreateEntity();
				projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
				projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
				projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 
					4); // z-index
				// the 4,4 are just the bounds of the sprite.
				projectile.AddComponent<BoxColliderComponent>(4, 4);
				projectileEmitter.lastEmissionTime = SDL_GetTicks();
			}
		}
	}

};