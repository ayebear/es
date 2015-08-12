// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_CORE_H
#define ES_CORE_H

#include <unordered_map>
#include <typeindex>
#include <es/internal/packedarray.h>
#include <es/componentpool.h>

namespace es
{

/*
Does things internally for the World by ID.
Contains a component pool and sets of components (entities).
Note: Does not depend on Entity, this does everything by ID.
    This way, Entity can depend on this, and get a reference to it.
It's named Core because it's the center of the World.
*/
struct Core
{
    Core() {}

    // Creates a new entity and returns its ID
    ID create(const std::string& name = "");

    // Copies an existing entity from another Core,
        // and returns its new ID from this Core.
    ID clone(Core& srcCore, ID srcId);

    // Get entity ID by name (creates a new entity if needed)
    ID operator[](const std::string& name);

    // Get entity ID by name (returns invalidId if name is invalid)
    ID get(const std::string& name) const;

    // Checks if an entity ID is valid
    bool isValid(ID id) const;

    // Removes an entity by its ID (but not its components)
    void remove(ID id);

    // Removes all entities
    void clear();

    // Register a name to an entity
    void setName(ID id, const std::string& name);

    // Returns an entity name by ID
    const std::string& getName(ID id) const;



    struct EntityData
    {
        EntityData(const std::string& name = ""): name(name) {}

        // The set of component IDs stored for an entity
        std::unordered_map<std::type_index, ID> compSet;

        // The entity name (optional)
        std::string name;
    };

    // All of the components for the entities in this core
    ComponentPool components;

    // All of the entities (component ID groups)
    PackedArray<EntityData> entities;

    // Maps names to entity IDs
    std::unordered_map<std::string, ID> entityNames;
};

}

#endif
