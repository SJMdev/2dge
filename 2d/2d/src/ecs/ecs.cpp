#include "ecs.hpp"
#include "../logger/logger.hpp"


int BaseComponent::nextId = 0;
// ENTITY
int Entity::GetId() const
{
	return id;
}



// SYSTEM
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


// REGISTRY
Entity Registry::CreateEntity()
{
	int entityId = numEntities++;
	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}
	Entity entity(entityId, this);
	entitiesToBeAdded.insert(entity);

	Logger::Log("Entity created with id = " + std::to_string(entityId));

	return entity;
}


void Registry::AddEntityToSystems(Entity entity) {
	const auto entityid = entity.GetId();
	
	auto entityComponentSignature = entityComponentSignatures[entityid];
	
	for (auto& [key, system]: systems)
	{
		const auto& systemComponentSignature = system->GetComponentSignature();
		
		// bitwise and, see if results are identical to system component signature.
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			// todo add the entity to the system.
			system->AddEntityToSystem(entity);
		}
	}
}


//
void Registry::Update()
{
	// at the end of the frame, actually kill entities and spawn new ones.
	for (auto entity : entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();
}