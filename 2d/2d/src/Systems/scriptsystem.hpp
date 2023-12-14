#pragma once

#include "../ecs/ecs.hpp"
#include "../components/scriptcomponent.hpp"
#include "../components/transformcomponent.hpp"

// first declare some native c++ fdunctions that we will bind to lua functions.
std::tuple<double,double> GetEntityPosition(Entity entity) {
	auto transform = entity.GetComponent<TransformComponent>();
	return std::make_tuple(transform.position.x, transform.position.y);
}

std::tuple<double, double> GetEntityVelocity(Entity entity) {
	auto rigidbody = entity.GetComponent<RigidBodyComponent>();
	return std::make_tuple(rigidbody.velocity.x, rigidbody.velocity.y);
}

void SetEntityVelocity(Entity entity, float x, float y) {
	auto& rigidbody = entity.GetComponent < RigidBodyComponent>();
	rigidbody.velocity = glm::vec2(x, y);
}

void SetEntityRotation(Entity entity, float orientation_degrees) {
	auto& transform = entity.GetComponent<TransformComponent>();
	transform.rotation = orientation_degrees;

}

void SetProjectileVelocity(Entity projectile, double x, double y) {
	auto& projectileComponent = projectile.GetComponent<ProjectileEmitterComponent>();
	projectileComponent.projectileVelocity = glm::vec2(x,y);
}

void SetEntityAnimationFrame(Entity entity, int frame) {
	auto& animation = entity.GetComponent<AnimationComponent>();
	animation.currentFrame = frame;
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

		lua.set_function("get_position", GetEntityPosition);
		lua.set_function("get_velocity", GetEntityVelocity);
		lua.set_function("set_velocity", SetEntityVelocity);
		lua.set_function("set_rotation", SetEntityRotation);
		lua.set_function("set_projectile_velocity", SetProjectileVelocity);
		lua.set_function("set_animation_frame", SetEntityAnimationFrame);


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