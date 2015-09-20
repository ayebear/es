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
    size_t size() const { return array.size(); }

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

        template <typename... Args>
        std::vector<TypeIndex> getTypeIndexes() const;

        template <typename... Args>
        std::vector<TypeIndex> getTypeIndexesString(const std::string& name, Args&&... args) const;

        void getTypeIndexString(std::vector<TypeIndex>& types, const std::string& name) const;

        template <typename... Args>
        void getTypeIndexString(std::vector<TypeIndex>& types, const std::string& name1, const std::string& name2, Args&&... args) const;

        template <typename T>
        void getTypeIndex(std::vector<TypeIndex>& types) const;

        template <typename A, typename B, typename... Args>
        void getTypeIndex(std::vector<TypeIndex>& types) const;

        EntityList queryTypes(std::vector<TypeIndex>& types);

        EntityList iterate(const std::type_index& minType, std::vector<TypeIndex>& types);

        Core core;

};

template <typename... Args>
std::vector<TypeIndex> World::getTypeIndexes() const
{
    std::vector<TypeIndex> types;
    getTypeIndex<Args...>(types);
    return types;
}

template <typename... Args>
std::vector<TypeIndex> World::getTypeIndexesString(const std::string& name, Args&&... args) const
{
    std::vector<TypeIndex> types;
    getTypeIndexString(types, name, args...);
    return types;
}

template <typename... Args>
void World::getTypeIndexString(std::vector<TypeIndex>& types, const std::string& name1, const std::string& name2, Args&&... args) const
{
    getTypeIndexString(types, name1);
    getTypeIndexString(types, name2, args...);
}

template <typename T>
void World::getTypeIndex(std::vector<TypeIndex>& types) const
{
    types.emplace_back(typeid(T));
}

template <typename A, typename B, typename... Args>
void World::getTypeIndex(std::vector<TypeIndex>& types) const
{
    getTypeIndex<A>(types);
    getTypeIndex<B, Args...>(types);
}

template <typename T, typename... Args>
World::EntityList World::query()
{
    // Get all type indexes from component types
    auto types = getTypeIndexes<T, Args...>();

    // Return list of entities with these component types
    return queryTypes(types);
}

template <typename... Args>
World::EntityList World::query(const std::string& name, Args&&... args)
{
    // Get all type indexes from component names
    auto types = getTypeIndexesString(name, args...);

    // Return list of entities with these component types
    return queryTypes(types);
}

template <typename T>
ComponentArrayIter<T> World::getComponents()
{
    return {core.components.get<T>()};
}

}

#endif
