#pragma once
#include "../ecs/ecs.hpp" 
#include "../eventbus/eventbus.hpp" 
#include "../events/keypressedevent.hpp"
#include "../components/spritecomponent.hpp"
#include "../components/rigidbodycomponent.hpp"

class KeyboardMovementSystem : public System {

public:
	// hold up, this does not really make sense to me.
	// why do I need to have a sprite or a rigid body to be able to move?
	// I would more need to imbue it with a "player controlled" component right?
	// the fact that I have a sprite or a rigid body is just auxiliary.
	// I find I have a hard time grasping which "requirecomponent" types
	// are needed for any particular system.
	// ah, this is not really necessary apparently. since we do not run 
	//update for this system. which SUCKS and makes things MORE convoluted for me.
	KeyboardMovementSystem() {
		RequireComponent<SpriteComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		std::string keyCode = std::to_string(event.symbol);
		std::string keySymbol(1, event.symbol);
		Logger::Log(" Key pressed event emitted: [" + keyCode + "]" + keySymbol + '\n');
	}

};