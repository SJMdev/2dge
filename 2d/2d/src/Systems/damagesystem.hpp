#pragma once
#include "../ecs/ecs.hpp" 
#include "../components/boxcollidercomponent.hpp"
#include "../eventbus/eventbus.hpp"
#include "../events/collisionevent.hpp" 
#include "../logger/logger.hpp"
#include <string>
#include "../components/projectilecomponent.hpp"
#include "../components/healthcomponent.hpp"


class DamageSystem : public System {

public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		
		Entity a = event.a;
		Entity b = event.b;
	/*	Logger::Log("The damage system received an event collision between entities " +
			std::to_string(event.a.GetId()) + " and entity " + std::to_string(event.b.GetId()) + '\n');*/

		// player- projectile interaction.
		if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
			// TODO:
			OnProjectileHitsPlayer(a, b); // 'a' is the projectile, 'b' is the player.
		}

		if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
			// TODO:
			OnProjectileHitsPlayer(b, a); // 'b' is the projectile, 'a' is the player.
		}

		// enemy - projectile interaction.
		if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
			OnProjectileHitsEnemy(a, b);
		}

		if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
			OnProjectileHitsEnemy(b, a);
		}


	}

	void OnProjectileHitsPlayer(Entity projectile, Entity player) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (!projectileComponent.isFriendly) {
			auto& health = player.GetComponent<HealthComponent>();
			// subtract the health of the player.
			health.healthPercentage -= projectileComponent.hitPercentDamage;

			// if we run out of health, kill me!
			if (health.healthPercentage <= 0) {
				player.Kill();
			}

			// always kill the projectile.

			projectile.Kill();
		}
	}

	void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (projectileComponent.isFriendly) {
			auto& health = enemy.GetComponent<HealthComponent>();

			health.healthPercentage -= projectileComponent.hitPercentDamage;

			if (health.healthPercentage <= 0) {
				enemy.Kill();
			}

			// always kill the projectile

			projectile.Kill();
		}
	}


	void Update() {
		// todo: what do we do then in the damage system????
	}
};