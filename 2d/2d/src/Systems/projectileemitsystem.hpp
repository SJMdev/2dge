#pragma once
#include "../ecs/ecs.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/projectileemittercomponent.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
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
			auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			auto transform  = entity.GetComponent<TransformComponent>();

			// check if it is time to emit a new projectile
			if (SDL_GetTicks() - projectileEmitter.lastEmissionTime){
				// todo: add a new projectile entity to the registry.
				Entity projectile = registry->CreateEntity();
				projectile.AddComponent<TransformComponent>(transform.position, glm::vec2(1.0, 1.0), 0.0);
				projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);

		}
	}

};