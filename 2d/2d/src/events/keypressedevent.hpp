#pragma once
#include "../ecs/ecs.hpp"
#include "../eventbus/event.hpp"



#include <SDL.h>
class KeyPressedEvent : public Event {
public:
	SDL_Keycode symbol;
	KeyPressedEvent(SDL_Keycode symbol) :
		symbol(symbol)
	{}

};