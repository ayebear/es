// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_ENTITY_H
#define ES_ENTITY_H

#include "es/component.h"
#include "es/componentpool.h"
#include "es/internal/core.h"

namespace es
{

/*
A proxy class created from World instances.
This is used for accessing/modifying components.
*/
class Entity
{
    public:

        Entity(Core& core, ID id = invalidId): core(core), id(id) {}


        // Assigning components ==============================================
        // Note: Component does not already have to exist for any of these

        // Sets a component from the passed in component
        template <typename T, typename... Args>
        Entity& assignFrom(const T& comp, Args&&... args);

        // Creates a new component and forwards constructor arguments
        template <typename T, typename... Args>
        Entity& assign(Args&&... args);

        // Same as assignFrom()
        template <typename T>
        Entity& operator<<(const T& comp);

        // Deserializes component name and data
        Entity& operator<<(const std::string& data);
        Entity& operator<<(const char* data);


        // Accessing components (No automatic creation) ======================
        // Note: When the component type doesn't exist, these return an
            // invalid handle, or nullptr.

        // Returns a handle to a specific component type
        template <typename T>
        Handle<ComponentArray<T>, T> get();

        // Returns a base component handle by component name
        Handle<BaseComponentArray, Component> get(const std::string& name);

        // Returns a specific component type pointer
        template <typename T>
        T* getPtr();

        // Returns a base component pointer by component name
        Component* getPtr(const std::string& name);

        // Writes a copy of the specified component type
        template <typename T>
        Entity& operator>>(T& comp);

        // Returns the names of all its components (only the ones with names)
        std::vector<std::string> getNames() const;


        // Accessing components (Automatic creation) =========================

        // Returns a handle to a specific component type
        template <typename T>
        Handle<ComponentArray<T>, T> at();

        // Returns a base component handle by component name
        Handle<BaseComponentArray, Component> at(const std::string& name);

        // Returns a reference to a specific component type
        template <typename T>
        T& access();

        // Returns a base component pointer by component name
        Component* accessPtr(const std::string& name);

        // Returns a base component reference by component name
        Component& access(const std::string& name);
        Component& operator[](const std::string& name);
        Component& operator[](const char* name);


        // Checking components ===============================================

        // Returns true if the entity has this component type
        template <typename T>
        bool has() const;

        // Returns true if the entity has all of the specified component types
        template <typename A, typename B, typename... Args>
        bool has() const;

        // Returns true if the entity has this component name
        bool has(const std::string& name) const;

        // Returns true if the entity has all of the specified component names
        template <typename... Args>
        bool has(const std::string& name, const std::string& name2, Args&&... args) const;

        // Returns number of component instances
        size_t total() const;

        // Returns true if there are no components
        bool empty() const;


        // Removing components ===============================================

        // Removes the specified component type
        template <typename T>
        void remove();

        // Removes all specified component types
        template <typename A, typename B, typename... Args>
        void remove();

        // Removes all specified component types by name
        void remove(const std::string& name);

        // Removes all specified component types by name
        template <typename... Args>
        void remove(const std::string& name, const std::string& name2, Args&&... args);

        // Removes all components
        void clear();


        // Entity copying ====================================================

        // Creates a copy of this entity, and returns it
        Entity clone(const std::string& newName = "") const;

        // Copies this entity into another world, and returns it
        Entity clone(Core& newCore, const std::string& newName = "") const;


        // Entity information ================================================

        // Returns the unique ID given by the Core
        ID getId() const;

        // Returns the assigned entity name (or an empty string)
        const std::string& getName() const;

        // Registers a name to this entity in the Core
        void setName(const std::string& name);

        // TODO: Consider this for keeping track of prototype names
        //const std::string& getType() const;

        // Sets the entity ID to invalid, but does not destroy it
        void invalidate();

        // Removes the entity and all of its components from the Core
        void destroy();

        // Returns true if this is a valid entity
        bool valid() const;

        // Returns true if this is a valid entity
        explicit operator bool() const;


        // Serialization =====================================================

        // Serializes all components into a vector of strings (with component names)
        std::vector<std::string> serialize() const;

        // Safely deserializes component name and data
        Entity& deserialize(const std::string& compName, const std::string& compData);

        // Deserializes component name and data (splits it)
        Entity& deserialize(const std::string& data);

        // Deserializes component name and data (splits each one)
        Entity& deserialize(const std::vector<std::string>& componentStrings);

    private:

        // Copies all components of an entity into another entity
        // void copyComponents(Entity& srcEnt, Entity& destEnt) const;
        void copyComponents(const Core& srcCore, ID srcId, Core& destCore, ID destId) const;

        // Get component ID from type, name, or type index
        template <typename T>
        ID getCompId() const;
        ID getCompId(const std::string& name) const;
        ID getCompId(const std::type_index& typeIdx) const;

        // Create/get component ID
        ID atCompId(const std::string& name);

        // Remove a component by type index
        void removeComp(const std::type_index& typeIdx);

        // For ending recursion
        void assignFrom() {}

        Core& core;
        ID id;
};

template <typename T, typename... Args>
Entity& Entity::assignFrom(const T& comp, Args&&... args)
{
    assign<T>(comp);
    assignFrom(args...);
    return *this;
}

template <typename T, typename... Args>
Entity& Entity::assign(Args&&... args)
{
    if (valid())
    {
        auto& compArray = core.components.get<T>();
        auto& compSet = core.entities[id].compSet;
        auto found = compSet.find(typeid(T));
        if (found == compSet.end())
            compSet[typeid(T)] = compArray.create(args...); // Create new component
        else
            compArray[found->second] = T(args...); // Assign existing component
    }
    return *this;
}

template <typename T>
Entity& Entity::operator<<(const T& comp)
{
    return assignFrom<T>(comp);
}

template <typename T>
Handle<ComponentArray<T>, T> Entity::get()
{
    return {&core.components.get<T>(), getCompId<T>()};
}

template <typename T>
T* Entity::getPtr()
{
    return core.components.get<T>().get(getCompId<T>());
}

template <typename T>
Entity& Entity::operator>>(T& comp)
{
    auto ptr = getPtr<T>();
    if (ptr)
        comp = *ptr;
    return *this;
}

template <typename T>
Handle<ComponentArray<T>, T> Entity::at()
{
    // TODO: Make this more efficient
    if (!has<T>())
        assign<T>();
    return get<T>();
}

template <typename T>
T& Entity::access()
{
    // TODO: Make this more efficient
    if (!has<T>())
        assign<T>();
    return *getPtr<T>();
}

template <typename T>
bool Entity::has() const
{
    return (getCompId<T>() != invalidId);
}

template <typename A, typename B, typename... Args>
bool Entity::has() const
{
    return (has<A>() && has<B, Args...>());
}

template <typename... Args>
bool Entity::has(const std::string& name, const std::string& name2, Args&&... args) const
{
    return (has(name) && has(name2, args...));
}

template <typename T>
void Entity::remove()
{
    removeComp(typeid(T));
}

template <typename A, typename B, typename... Args>
void Entity::remove()
{
    remove<A>();
    remove<B, Args...>();
}

template <typename... Args>
void Entity::remove(const std::string& name, const std::string& name2, Args&&... args)
{
    remove(name);
    remove(name2, args...);
}

template <typename T>
ID Entity::getCompId() const
{
    return getCompId(typeid(T));
}

}

#endif
