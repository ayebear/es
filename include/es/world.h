// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_WORLD_H
#define ES_WORLD_H

#include <es/internal/core.h>
#include <es/entity.h>

namespace es
{

// Wraps a component array into an iterable object
// This is so component arrays cannot be directly modified
template <class T>
struct ComponentArrayIter
{
    ComponentArrayIter(ComponentArray<T>& array): array(array) {}

    auto begin() { return array.begin(); }
    auto end() { return array.end(); }
    auto cbegin() const { return array.cbegin(); }
    auto cend() const { return array.cend(); }

    private:
        ComponentArray<T>& array;
};

/*
A wrapper class around Core and Entity.
Creates instances of Entity by constructing it with ID and Core&.
*/
class World
{
    public:

        World() {}


        // Creating entities =================================================

        // Creates an empty entity and returns it
        Entity create(const std::string& name = "");

        // Creates an entity from a prototype (same as clone)
        Entity copy(const std::string& prototypeName, const std::string& name = "");

        // Creates an entity from a prototype (same as copy)
        Entity clone(const std::string& prototypeName, const std::string& name = "");


        // Creates a new entity if needed ====================================

        // Get entity by name
        Entity operator[](const std::string& name);

        // Get entity by name (and create from prototype if it doesn't exist)
        Entity operator()(const std::string& prototypeName, const std::string& name);


        // Returns an invalid Entity if it doesn't exist =====================

        // Get entity by ID
        Entity operator[](ID id);

        // Get entity by name
        Entity get(const std::string& name);

        // Get entity by ID
        Entity get(ID id);

        // Get entity from component (with owner ID)
        Entity from(const Component& comp);


        // Remove entities ===================================================

        // Remove an entity by ID
        void destroy(ID id);

        // Remove an entity by name
        void destroy(const std::string& name);

        // Removes all entities
        void clear();


        // Query entities and components =====================================

        using EntityList = std::vector<Entity>;

        // Returns all entities
        EntityList query();

        // Returns entities with specified component types
        template <typename T, typename... Args>
        EntityList query();

        // Returns entities with specified component names
        template <typename... Args>
        EntityList query(const std::string& name, Args&&... args);

        // Used for iterating directly through components
        template <typename T>
        ComponentArrayIter<T> getComponents();


        // Iterate through all entities ======================================

        // TODO: Add begin/end and const versions
            // This should iterate through the entities safely and return an
            // Entity handle each time.


        // Miscellaneous =====================================================

        // Returns true if there is a valid entity with this ID
        bool valid(ID id) const;

        // Returns true if there is a valid entity with this name
        bool valid(const std::string& name) const;

        // Returns the number of entities in the world
        size_t size() const;

        // Implicit cast for accessing the core
        operator Core&();

        // Returns true if the component name is valid
        static bool validName(const std::string& compName);

        static World prototypes;

    private:

        Core core;

};

template <typename T, typename... Args>
World::EntityList World::query()
{
    EntityList entities;
    for (auto id: core.entities.getIndex())
    {
        Entity ent {core, id};
        if (ent.has<T, Args...>())
            entities.push_back(ent);
    }
    return entities;
}

template <typename... Args>
World::EntityList World::query(const std::string& name, Args&&... args)
{
    EntityList entities;
    for (auto id: core.entities.getIndex())
    {
        Entity ent {core, id};
        if (ent.has(name, args...))
            entities.push_back(ent);
    }
    return entities;
}

template <typename T>
ComponentArrayIter<T> World::getComponents()
{
    return {core.components.get<T>()};
}

}

#endif
