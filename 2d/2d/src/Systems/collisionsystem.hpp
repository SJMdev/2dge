#pragma once
#include "../ecs/ecs.hpp"
#include "../components/boxcollidercomponent.hpp"
#include "../eventbus/eventbus.hpp"
#include "../eventbus/event.hpp"
#include "../eventbus/event.hpp"
#include "../events/collisionevent.hpp"


class CollisionSystem : public System {


public:
	CollisionSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(std::unique_ptr<EventBus>& eventBus) {
		// TODO: 
		// check all entities that have a boxcollider
		// to see if they are colliding with each other.

		auto entities = GetSystemEntities();
		// Loop all the entities that the system is interested in
		for (auto i = entities.begin(); i != entities.end(); i++) {
			Entity a = *i;
			auto lhs_transform = a.GetComponent<TransformComponent>();
			auto lhs_collider = a.GetComponent<BoxColliderComponent>();

			// Loop all the entities that still need to be checked (to the right of i)
			for (auto j = i; j != entities.end(); j++) {
				Entity b = *j;
				if (a == b)
				{
					continue;
				}

				auto rhs_transform = b.GetComponent<TransformComponent>();
				auto rhs_collider = b.GetComponent<BoxColliderComponent>();
			
				// Perform the AABB collision check between entities a and b
				bool collisionHappened = CheckAABBCollision(
					lhs_transform.position.x + lhs_collider.offset.x,
					lhs_transform.position.y + lhs_collider.offset.y,
					lhs_collider.width,
					lhs_collider.height,
					rhs_transform.position.x + rhs_collider.offset.x,
					rhs_transform.position.y + rhs_collider.offset.y,
					rhs_collider.width,
					rhs_collider.height
				);

				if (collisionHappened)
				{
					Logger::Log("Entity " + std::to_string(a.GetId()) + "has collided with Entity " + std::to_string(b.GetId()));
					
					eventBus->EmitEvent<CollisionEvent>(a, b);
				}

			}
		}
	}

	bool CheckAABBCollision(
		double aX, double aY, double aW, double aH,
		double bX, double bY, double bW, double bH)
	{
		return (
			aX < bX + bW &&
			aX + aW > bX &&
			aY < bY + bH &&
			aY + aH > bY
			);
	}
};