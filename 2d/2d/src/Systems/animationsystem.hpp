#pragma once
#include "../ecs/ecs.hpp"
#include "../components/spritecomponent.hpp"
#include "../components/animationcomponent.hpp"

class AnimationSystem : public System
{
public:
	 AnimationSystem() {
		RequireComponent<SpriteComponent>();
		RequireComponent<AnimationComponent>();
	}
	 ~AnimationSystem() = default;


	 void Update() {
		 for (auto entity : GetSystemEntities()) {
			 auto& animation = entity.GetComponent<AnimationComponent>();
			 auto& sprite = entity.GetComponent<SpriteComponent>();

			 animation.currentFrame = (
				 (SDL_GetTicks() - animation.startTime)
				 * animation.frameSpeedRate / 1000) % animation.numFrames;
			 // maybe advance to the next animation frame
			 sprite.srcRect.x = animation.currentFrame * sprite.width;
		 }
	 }
};