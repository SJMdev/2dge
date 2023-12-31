#pragma once
#include <SDL.h>

struct AnimationComponent
{
	int numFrames;
	int currentFrame;
	int frameSpeedRate; // uh?
	bool isLoop;
	int startTime;


	AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool isLoop = true)
		:
		numFrames(numFrames),
		currentFrame(0),
		frameSpeedRate(frameSpeedRate), //how many frames of animation each second?
		isLoop(isLoop),
		startTime(SDL_GetTicks())
	{
	}
	
};