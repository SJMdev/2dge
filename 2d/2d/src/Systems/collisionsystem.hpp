#pragma once
#include "../ecs/ecs.hpp"
#include "../components/boxcollidercomponent.hpp"
class CollisionSystem : public System {


public:
	CollisionSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update() {
		// TODO: 
		// check all entities that have a boxcollider
		// to see if they are colliding with each other.
		size_t entity_count = GetSystemEntities().size();

		for (auto a: GetSystemEntities())
		{
			
			auto lhs_transform = a.GetComponent<TransformComponent>();
			auto lhs_collider = a.GetComponent<BoxColliderComponent>();
			for (auto b: GetSystemEntities())
			{
				if (a == b)
				{
					continue;
				}

				auto rhs_transform = b.GetComponent<TransformComponent>();
				auto rhs_collider = b.GetComponent<BoxColliderComponent>();
				
				bool have_collided = CheckAABBCollision(
					lhs_transform.position.x,
					lhs_transform.position.y,
					lhs_collider.width,
					lhs_collider.height,
					rhs_transform.position.x,
					rhs_transform.position.y,
					rhs_collider.width,
					rhs_collider.height
				);

				if (have_collided)
				{
					Logger::Log("Entity " + std::to_string(a.GetId()) + "has collided with Entity " + std::to_string(b.GetId()));
					
					//TODO: emit an event.
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