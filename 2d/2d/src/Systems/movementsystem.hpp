#pragma once
#include "../ecs/ecs.hpp"

#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/spritecomponent.hpp"

#include "../logger/logger.hpp"
#include "../eventbus/eventbus.hpp"
#include "../events/collisionevent.hpp"

class MovementSystem : public System {
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}
	~MovementSystem() = default;
	MovementSystem(MovementSystem& rhs) = delete;
	MovementSystem(MovementSystem&& rhs) = delete;

	void Update(double deltaTime) {

		// loop all entities that the system is interested in.
		for (auto& entity : GetSystemEntities()) {
			// update entity position based on its velocity 
			// every frame of the game loop.
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			// prevent the main player from moving outside the map boundaries
			if (entity.HasTag("player")) {
				int paddingLeft = 10;
				int paddingTop = 10;
				int paddingRight = 50;
				int paddingBottom = 50;

				transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
				transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
				transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y; 
				transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : transform.position.y;

			}



			bool isEntityOutsideMap = (
				transform.position.x < 0 ||
				transform.position.x > Game::mapWidth ||
				transform.position.y  < 0 ||
				transform.position.y > Game::mapHeight
				);

			// kill all entities that move outside the map boundaries
			if (isEntityOutsideMap && !entity.HasTag("player")) {
				entity.Kill();
			}
			//Logger::Log("Entity id =" +
			//	std::to_string(entity.GetId()) +
			//	" position is now (" + 
			//	std::to_string(transform.position.x) +
			//	std::to_string(transform.position.y) + ")" + '\n');
		}
	}

	void onEnemyHitsObstacle(Entity enemy, Entity obstacle) {
		
		if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
			auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();

			if (rigidBody.velocity.x != 0) {
				rigidBody.velocity *= -1;
				sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}
			if (rigidBody.velocity.y != 0) {
				rigidBody.velocity *= -1;
				sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
	}

	void OnCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;

		if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
			onEnemyHitsObstacle(a, b);
		}
		if(a.BelongsToGroup("obstacles") && a.BelongsToGroup("enemies")) {
			onEnemyHitsObstacle(b, a);
		}
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
	}



};