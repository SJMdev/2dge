#pragma once
#include "../ecs/ecs.hpp"
#include "../eventbus/event.hpp"


class CollisionEvent: public Event
{
public:
	Entity a;
	Entity b;
	CollisionEvent(Entity a, Entity b)
	:
		a(a),
		b(b)
	{

	}
};