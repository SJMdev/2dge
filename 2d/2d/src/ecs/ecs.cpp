#include "ecs.hpp"

int iComponent::nextId = 0;

int Entity::GetId() const
{
	return id;
}

bool Entity::operator==(const Entity& e) const
{
	return id == e.id;
}

bool Entity::operator!=(const Entity& e) const
{
	return id == e.id;
}

// should we check if the entity does not already exist?
void System::AddEntityToSystem(Entity entity) {
	entities.push_back(entity);
}
void System::RemoveEntityFromSystem(Entity entity) {
	std::erase(entities, entity);
}
std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}
const Signature& System::GetComponentSignature() const
{
	return componentSignature;
}


Entity Registry::CreateEntity()
{
	int entityId = numEntities++;
	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}
	Entity entity(entityId);
	entitiesToBeAdded.insert(entity);

	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}

void Registry::Update()
{
	// at the end of the frame, actually kill entities and spawn new ones.
}