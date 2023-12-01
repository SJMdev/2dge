#pragma once
#include "../ecs/ecs.hpp"
#include "../components/camerafollowcomponent.hpp"
#include <string>
class CameraMovementSystem : public System {
public:
	CameraMovementSystem() {

		RequireComponent<CameraFollowComponent>();
		RequireComponent<TransformComponent>();

	}

	void Update(SDL_Rect& camera) {
		for (auto entity : GetSystemEntities()) {
            auto transform = entity.GetComponent<TransformComponent>();

            // this is quite involved, but it boils down to trying to keep the camera in bounds.

            if (transform.position.x + (camera.w / 2) < Game::mapWidth) {
                camera.x = transform.position.x - (Game::windowWidth / 2);
            }

            if (transform.position.y + (camera.h / 2) < Game::mapHeight) {
                camera.y = transform.position.y - (Game::windowHeight / 2);
            }

            // Keep camera rectangle view inside the screen limits
            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = (camera.x + camera.w > Game::mapWidth) ? Game::mapWidth - camera.w : camera.x;
            camera.y = (camera.y + camera.h > Game::mapHeight) ? Game::mapHeight - camera.h : camera.y;

			Logger::Log("Camera changed its position to: "+  std::to_string(camera.x) + ", " +  std::to_string(camera.y) + '\n');
		}
	}

};