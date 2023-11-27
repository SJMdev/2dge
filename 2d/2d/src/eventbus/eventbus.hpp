#pragma once

class EventBus {

public:
	EventBus() = default;
	~EventBus() = default;



	// subscribe to an event type <Tevent>
	// in our implementation, a listener subscribes to an event.
	// exmaple./eventBus->SubscribeToEvent<CollisionEvent>(&Game::onCollision);
	template <typename TEvent>
	void SubcribeToEvent() {

	}

	// emit an event of type <TEvent>
	// as soon as something emits an event we go ahead and execute all the listener
	// callback functions.
	//example: eventbus->EmitEvent<CollisionEvent>(player, enemy);
	template <typename TEvent>
	void EmitEvent() {

	}
};