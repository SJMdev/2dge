#pragma once

#include "../ecs/ecs.hpp"
#include "../components/scriptcomponent.hpp"
#include "../components/transformcomponent.hpp"

// first declare some native c++ fdunctions that we will bind to lua functions.
glm::vec2 GetEntityPosition(Entity entity) {
	auto transform = entity.GetComponent<TransformComponent>();
	return transform.position;
}

void SetEntityPosition(Entity entity, double x, double y) {

	if (entity.HasComponent<TransformComponent>()) {
		auto& transform = entity.GetComponent<TransformComponent>();
		transform.position.x = x;
		transform.position.y = y;
	}
	else {
		Logger::Err("trying to set position of an entity that has no transform component.");
	}
	
}

class ScriptSystem : public System {

public:
	ScriptSystem() {
		RequireComponent<ScriptComponent>();
	}

	void CreateLuaBindings(sol::state& lua) {
		// create the "entity"usertype so lua knows what entity is
		lua.new_usertype<Entity>(
			"entity",
			"get_id",           &Entity::GetId,
			"destroy",          &Entity::Kill,
			"has_tag",          &Entity::HasTag,
			"belongs_to_group", &Entity::BelongsToGroup
		);


		// create all the bindings between C++ and lua functions
		lua.set_function("set_position", SetEntityPosition);
	}

	void Update(double deltaTime, int ellapsedTime) {
		for (auto entity : GetSystemEntities()) {
			const auto script = entity.GetComponent<ScriptComponent>();
			script.func(entity, deltaTime, ellapsedTime);
		}
	}

		
};