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
		RequireComponent<KeyboardControlledComponent>();
		RequireComponent<SpriteComponent>();
		RequireComponent<RigidBodyComponent>();
	}


	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		// TODO: change the sprite and the velocity of my entity
		// the sprite is laid out in a fashion where left to right are frames of animation
		// and top to bottom is up, left, down, right facing animations.
		// therefore if we press a button, we move the y part of the srcRect (i.e. where we look
		// in the original sprite sheet to get the 32 * 32 subsection from).
		// the x part is taken care of the animation system, since we move x frames per second there.
		// with modulo. 
		for (auto entity : GetSystemEntities()) {
			const auto keyboardcontrol = entity.GetComponent<KeyboardControlledComponent>();
			auto& sprite = entity.GetComponent<SpriteComponent>();
			auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

			switch (event.symbol)
			{
				case SDLK_UP: {
					rigidbody.velocity = keyboardcontrol.upVelocity;
					sprite.srcRect.y = sprite.height * 0;
					break;
				}
				case SDLK_RIGHT: {
					rigidbody.velocity = keyboardcontrol.rightVelocity;
					sprite.srcRect.y = sprite.height * 1;
					break;
				}
				case SDLK_DOWN: {
					rigidbody.velocity = keyboardcontrol.downVelocity;
					sprite.srcRect.y = sprite.height * 2;
					break;
				}
				case SDLK_LEFT: {
					rigidbody.velocity = keyboardcontrol.leftVelocity;
					sprite.srcRect.y = sprite.height * 3;
					break;
				}
			}

		}
	}

};