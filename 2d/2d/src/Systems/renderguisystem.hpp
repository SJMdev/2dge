#pragma once
#include "../ecs/ecs.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include "../components/spritecomponent.hpp"
#include "../components/transformcomponent.hpp"
#include "../components/rigidbodycomponent.hpp"
#include "../components/boxcollidercomponent.hpp"
#include "../components/projectileemittercomponent.hpp"
#include "../components/healthcomponent.hpp"

class RenderGUISystem : public System {
public:

	RenderGUISystem() = default;

	void Update(std::unique_ptr<Registry>& registry, const SDL_Rect& camera)
	{

		ImGui::NewFrame();
		// TODO: draw all the imgui objects in the screen.
		// TODO: enemy x y position, enemy scale, enemy rotation
		// TODO: enemy x & y velocity.
		//ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;
		if(ImGui::Begin("Spawn enemies"))
		{
			static int posX = 0;
			static int posY = 0;
			static int scaleX = 1;
			static int scaleY = 1;
			static int velX = 0;
			static int velY = 0;
			static float rotation = 0.0;
			static float projAngle = 0.0;
			static float projSpeed = 100.0;
			static int projRepeat = 10;
			static int projDuration = 10;
			const char* sprites[] = {"tank-image", "truck-image"};
			static int selectedSpriteIndex = 0;
			static int health = 100;










			// section to input enemy sprite texture id
			if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
			}
			ImGui::Spacing();

			// section to input enemy transform values
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputInt("X position", &posX);
				ImGui::InputInt("Y position", &posY);
				ImGui::SliderInt("scale X", &scaleX, 1, 10);
				ImGui::SliderInt("scale Y", &scaleY, 1, 10);
				ImGui::SliderAngle("rotation(deg", &rotation, 0, 360);
			}
			ImGui::Spacing();


			// section to input enemy rigid body values.
			if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputInt("velocity x", &velX);
				ImGui::InputInt("velocity y", &velY);
			}
			ImGui::Spacing();


		// section to input enemy projectile emitter values.
			if (ImGui::CollapsingHeader("projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
				ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
				ImGui::InputInt("repeat (sec)", &projRepeat);
				ImGui::InputInt("duration (sec)", &projDuration);
			}

			ImGui::Spacing();

			// sec tion to input enemy health values.

			if (ImGui::CollapsingHeader("health"), ImGuiTreeNodeFlags_DefaultOpen) {
				ImGui::SliderInt("%", &health, 0, 100);
			}

			// enemy creation button
			if (ImGui::Button("Create new enemy")) {
				Entity enemy = registry->CreateEntity();
				enemy.Group("enemies");
				enemy.AddComponent<TransformComponent>(
					glm::vec2(posX, posY),
					glm::vec2(scaleX, scaleY),
					glm::degrees(rotation));

				enemy.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
				enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 1);
				enemy.AddComponent<BoxColliderComponent>(25, 20, glm::vec2(5,5));
				
				// convert from angle to x / y value.
				double projVelX = cos(projAngle) * projSpeed;
				double projVelY = sin(projAngle) * projSpeed;

				enemy.AddComponent<ProjectileEmitterComponent>(
					glm::vec2(projVelX, projVelY),
					projRepeat * 1000, // every n seconds in miliseconds
					projDuration * 100,
					10, // percent hit damage
					false);
				enemy.AddComponent<HealthComponent>(health);

				// reset most input values arfter we create a new enemy
				posX = posY = scaleX = scaleY = rotation = projAngle = 0;
				scaleX = scaleY = 1.0;
				projRepeat = projDuration = 10;
				projSpeed = 100;
				health = 100;
			}
		}
		ImGui::End();

		// display a small overlay window to display the map position using the mouse
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::SetNextWindowBgAlpha(0.9f);
		if (ImGui::Begin("map coordinates", NULL, windowFlags)) {
			ImGui::Text(
				"map Coordinates (x=%.1f, y = %.1f)",
				ImGui::GetIO().MousePos.x + camera.x,
				ImGui::GetIO().MousePos.y + camera.y
			);

		}
		ImGui::End();

		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
	}

};