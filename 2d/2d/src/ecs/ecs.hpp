#pragma once
#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <set>

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
	static int getId() {
		static auto id = nextId++;
		return id;
	}
};

class Entity {
private:
	int id;

public:
	Entity(int id)
	:
		id(id)
	{

	}
	Entity(const Entity& entity) = default;
	~Entity() = default;
	
	Entity& operator=(const Entity& other) = default;
	bool operator==(const Entity& other) const;
	bool operator!=(const Entity& other) const;
	int GetId() const;

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
	virtual ~IPool();
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
	int GetSize() const { return data.size(); }
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
	std::vector<IPool*> componentPools;

	// which components are turned "on" for each entity. Entity is used as an index.
	std::vector<Signature> entityComponentSignatures;

	// map of active systems[index = system typeid]
	std::unordered_map<std::type_index, System*> systems;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

public:
	Registry() = default;
	
	Entity CreateEntity();
	void KillEntity(Entity entity);
	void AddEntityToSystem(Entity entity);

	// Component things
	template <typename T, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template <typename T>
	void RemoveComponent(Entity entity);

	template <typename T>
	bool HasComponent(Entity entity) const;

	//template <typename T>
	//T& GetComponent(Entity entity) const;

	// System things.
	template <typename T, typename ...TArgs> 
	void AddSystem(TArgs&& ...args);
	template <typename T>
	/*RemoveSystem();
	HasSystem();
	GetSystem();*/

	void AddSystem();
	void Update();
};



template <typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();
	
	// if we have not ever added this copmonent to any entity.
	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	// if we still don't have a pool for that component type
	if (!componentPools[componentId]) {
		Pool<T>* newComponentPool = new Pool<T>();
		componentPools[componentId] = newComponentPool;
	}

	// get the pool of compnent values for that component type
	Pool<T>* componentPool = Pool<T>(componentPools[componentId]);

	// this is actually a very weird implementation. if entity is an index, this will onyl grow
	// forever and we will never reuse empty fields.
	if (entityId >= componentPool->GetSize()) {
		componentPool->Resize(numEntities);
	}

	// create a new component object of the type T, and forward the various parameters to the constructor.
	T newComponent(std::forward<TArgs>(args)...);

	// add the component to the component pool list, using the entity id as index.
	componentPool->Set(entityId, newComponent);

	// finally, change the component signature of the entity and set the component id in the bitset.
	entityComponentSignatures[entityId].set(componentId);
}

template <typename T>
void Registry::RemoveComponent(Entity entity)
{
	const auto entityId = entity.GetId();
	const auto componentId = Component<T>::GetId();
	entityComponentSignatures[entityId].set(componentId, false);
}

template <typename T>
bool Registry::HasComponent(Entity entity) const
{
	const auto entityId = entity.GetId();
	const auto componentId = Component<T>::GetId();

	return entityComponentSignatures[entityId].test(componentId);
}