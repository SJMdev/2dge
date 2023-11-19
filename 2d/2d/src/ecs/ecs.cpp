#include "ecs.hpp"

int Entity::GetId() const
{
	return id;
}


// should we check if the entity does not already exist?
void System::addEntityToSystem(Entity entity) {
	entities.push_back(entity);
}
void System::removeEntityFromSystem(Entity entity) {
	entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}
std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}
const Signature& System::getComponentSignature() const
{
	return componentSignature;
}