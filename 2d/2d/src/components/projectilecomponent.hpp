#pragma once
#include <SDL.h>


struct ProjectileComponent {
	bool isFriendly;
	int hitPercentDamage;
	int duration;
	int startTime;

	ProjectileComponent(
		bool isFriendly = false,
		int hitPercentDamage = 0,
		int duration = 0
	) :
		isFriendly(isFriendly),
		hitPercentDamage(hitPercentDamage),
		duration(duration),
		startTime(SDL_GetTicks())
	{}
};