#pragma once
#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include <string>
#include "../logger/logger.hpp"

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;
// we use a bitset| (1s and 0s) to kee ptrack of which components an entity has,
// which also helps keep track of which entities a system is interested in.

struct BaseComponent {
protected:
	static int nextId;
};

// used to assign a unique ID to a component type
template<typename T> 
struct Component: BaseComponent
{
	// returns the unqiue ID of the component<T> (uh, if we call this multiple times,
	// we will just increase the next id?
	// NO, since static gets initialized once. This is super convoluted!
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};

class Registry;

class Entity {
private:
	int id;

public:
	Entity(int id, Registry* registry)
	:
		id(id),
		registry(registry)
	{

	}
	Entity(const Entity& entity) = default;
	~Entity() = default;
	void Kill();
	
	bool operator ==(const Entity& other) const { return id == other.id; }
	bool operator !=(const Entity& other) const { return id != other.id; }
	bool operator >(const Entity& other) const { return id > other.id; }
	bool operator <(const Entity& other) const { return id < other.id; }
	int GetId() const;

	// hold a pointer to the entity's owner registry
	Registry* registry;

	template <typename TComponent, typename ...TArgs>
	void AddComponent(TArgs&& ...args);

	template <typename TComponent>
	void RemoveComponent();
	
	template <typename TComponent>
	bool HasComponent() const;
	
	template <typename TComponent>
	TComponent& GetComponent() const;

};

class System {
	// which components an entity must have for the system to consider the entity.
	Signature componentSignature;
	// list of all the entities that the System is interested in.
	std::vector<Entity> entities;

public:
	System() = default;
	~System() = default;
	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;
	
	// define the component type T that entities must have to be considered
	// by the system.
	// "RequireComponent" is a horrible name. It should be like: add_component_to_signature
	// or something like that.
	template <typename TComponent> void RequireComponent();
};

template <typename TComponent> void System::RequireComponent() {
	const auto componentId = Component<TComponent>::GetId();
	componentSignature.set(componentId);
}


// we define a purely virtual interface that does is not a template 
// so we can have a vector of pointers to this type (i.e. sort of type-erasure).
// I guess we will be converting it back to the templated variant at some point
// when we retrieve that particular vector?
// https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors:
// "Virtual destructors are useful when you might
// potentially delete an instance of a derived class through a pointer to base class:"
// ok, makes sense.
class IPool {
public:
	virtual ~IPool() {}
};

template<typename T>
class Pool: public IPool
{
private:
	std::vector<T> data;

public:
	Pool(int size = 100) {
		Resize(size);
	}
	virtual ~Pool() = default;

	bool isEmpty() const { return data.empty(); }
	int GetSize() const { return static_cast<int>(data.size()); }
	void Resize(int n) { data.resize(n); }
	void Clear() { data.clear(); };
	void Add(T object) {
		data.push_back(object);
	}
	void Set(int index, T object) { data[index] = object; }
	T& Get(int index) { return static_Cast<T&>(data[index]);}
	T& operator[](unsigned int index) { return data[index]; };
};


// the registry manages the creation and destruction of entities,
// as well as adding systems and adding components to entities.
class Registry {
private:


	// keep track of how many entities were aded to the scene.
	int numEntities = 0;
	 // vector index = component type id;
	// pool index = entity ID;
	std::vector<std::shared_ptr<IPool>> componentPools;

	// which components are turned "on" for each entity. Entity is used as an index.
	std::vector<Signature> entityComponentSignatures;

	// map of active systems[index = system typeid]
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Lsit of free ids that were previously removed
	std::deque<int> freeIds;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

public:
	Registry() = default;
	Entity CreateEntity();
	void KillEntity(Entity entity);
	
	
	// checks the component signature of an entity and add the entity to the systems
	// that are interested in it.
	void AddEntityToSystems(Entity entity);
	// remove entity from its systems.
	void RemoveEntityFromSystems(Entity entity);

	// Component things
	template <typename T, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template <typename T>
	void RemoveComponent(Entity entity);

	template <typename T>
	bool HasComponent(Entity entity) const;

	template <typename T> 
	T& GetComponent(Entity entity) const;

	// System things.
	template <typename T, typename ...TArgs> 
	void AddSystem(TArgs&& ...args);

	template <typename T> 
	void RemoveSystem();

	template <typename T>
	bool HasSystem() const;
	
	template <typename T>
	T& GetSystem() const;

	void Update();
};

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>());
		componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->Set(entityId, newComponent);

	entityComponentSignatures[entityId].set(componentId);

	Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}




template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);

}

template <typename TComponent>
void Entity::RemoveComponent()
{
	registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>(*this);
}



template <typename T>
void Registry::RemoveComponent(Entity entity)
{
	const auto entityId = entity.GetId();
	const auto componentId = Component<T>::GetId();
	entityComponentSignatures[entityId].set(componentId, false);

	Logger::Log("Component id = " + std::to_string(componentId) + "was removed from entity id = " + std::to_string(entityId));

}

template <typename T>
bool Registry::HasComponent(Entity entity) const
{
	const auto entityId = entity.GetId();
	const auto componentId = Component<T>::GetId();

	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	return (*componentPool)[entityId];
}


template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
	auto  newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}


template <typename TSystem>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}


template <typename TSystem>
bool Registry::HasSystem() const {
	return systems.find(std::type_index(tyeipd(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

