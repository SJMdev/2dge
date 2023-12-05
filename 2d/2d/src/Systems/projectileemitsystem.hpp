#pragma once
#include "../ecs/ecs.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/projectileemittercomponent.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/boxcollidercomponent.hpp"
#include "../components/projectilecomponent.hpp"

class ProjectileEmitSystem : public System {
public:
	ProjectileEmitSystem()
	{
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		if (event.symbol == SDLK_SPACE) {
			for (auto entity : GetSystemEntities()) {
				// select only the player.
				if (entity.HasTag("player")) {
					const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
					const auto transform = entity.GetComponent<TransformComponent>();
					const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

					glm::vec2 projectilePosition = transform.position;
					if (entity.HasComponent<SpriteComponent>())
					{
						const auto sprite = entity.GetComponent<SpriteComponent>();
						projectilePosition.x += (transform.scale.x * sprite.width / 2);
						projectilePosition.y += (transform.scale.y * sprite.height / 2);
					}

					glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
					int directionX = 0;
					int directionY = 0;
					if (rigidbody.velocity.x > 0) directionX = +1;
					if (rigidbody.velocity.x < 0) directionX = -1;
					if (rigidbody.velocity.y > 0) directionY = +1;
					if (rigidbody.velocity.y > 0) directionY = -1;
					projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
					projectileVelocity.y = projectileEmitter.projectileVelocity.y * directionY;


					// add a new projectile entity to the registry.
					Entity projectile = entity.registry->CreateEntity();
					projectile.Group("projectiles");
					projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
					projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
					projectile.AddComponent<SpriteComponent>(
						"bullet-image",
						4, 4,  // width, height
						4); // z-index
					projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly,
						projectileEmitter.hitPercentDamage
						, projectileEmitter.projectileDuration);
					// the 4,4 are just the bounds of the sprite.
					projectile.AddComponent<BoxColliderComponent>(4, 4);

				}


			}
		}
	}

	void Update(std::unique_ptr<Registry>& registry) {
		// actually emit a projectile ?
		for (auto entity : GetSystemEntities()) {
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto transform  = entity.GetComponent<TransformComponent>();

			if (projectileEmitter.repeatFrequency == 0) {
				continue;
			}

			// check if it is time to emit a new projectile
			if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency) {
				glm::vec2 projectilePosition = transform.position;
				if (entity.HasComponent<SpriteComponent>())
				{
					// I do not like how scale is used throughout instead of multiplying the dimensions once.
					// it seems like things can go wrong very fast.
					const auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePosition.x += (transform.scale.x * sprite.width / 2);
					projectilePosition.y += (transform.scale.y * sprite.height / 2);
				}
				// todo: add a new projectile entity to the registry.
				Entity projectile = registry->CreateEntity();
				projectile.Group("projectiles");
				projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
				projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
				projectile.AddComponent<SpriteComponent>(
					"bullet-image", 
					4, 4,  // width, height
					4); // z-index
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly,projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
				// the 4,4 are just the bounds of the sprite.
				projectile.AddComponent<BoxColliderComponent>(4, 4);
				projectileEmitter.lastEmissionTime = SDL_GetTicks();
			}
		}
	}


};