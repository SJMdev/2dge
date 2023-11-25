#pragma once
#include "../ecs/ecs.hpp"

#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../logger/logger.hpp"

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

			//Logger::Log("Entity id =" +
			//	std::to_string(entity.GetId()) +
			//	" position is now (" + 
			//	std::to_string(transform.position.x) +
			//	std::to_string(transform.position.y) + ")" + '\n');
		}
	}
};