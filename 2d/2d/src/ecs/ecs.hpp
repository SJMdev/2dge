#pragma once
#include <bitset>

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
	~Entity() = default;
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
	void addEntityToSystem(Entity entity);
	void removeEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& getComponentSignature() const;
	
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


class Registry {

};