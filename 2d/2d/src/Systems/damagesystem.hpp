#pragma once
#include "../ecs/ecs.hpp" 
#include "../components/boxcollidercomponent.hpp"
#include "../eventbus/eventbus.hpp"
#include "../events/collisionevent.hpp" 
#include "../logger/logger.hpp"
#include <string>

class DamageSystem : public System {

public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Logger::Log("The damage system received an event collision between entities " +
			std::to_string(event.a.GetId()) + " and entity " + std::to_string(event.b.GetId()) + '\n');
		//event.a.Kill();
		//event.b.Kill();
	}
	void Update() {
		// todo: what do we do then in the damage system????
	}
};