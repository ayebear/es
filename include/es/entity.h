// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_ENTITY_H
#define ES_ENTITY_H

#include "es/component.h"
#include "es/componentpool.h"

namespace es
{

class World;

/*
Created from World instances.
This is the interface for accessing/modifying components.
*/
class Entity
{
    public:

        Entity(World& world, ID id = invalidId): world(world), id(id) {}


        // Assigning components ==============================================
        // Note: Component does not already have to exist for any of these

        // Sets a component from the passed in component
        template <typename T>
        Entity& assign(const T& comp);

        // Creates a new component and forwards constructor arguments
        template <typename T, typename... Args>
        Entity& assign(Args&&... args);

        // Same as value based assign method
        template <typename T>
        Entity& operator<<(const T& comp);


        // Accessing components ==============================================

        // Returns a specific component type pointer
        // Note: Returns nullptr if component does not exist
        template <typename T>
        T* get();

        // Returns a base component pointer from bound string name
        // Note: Returns nullptr if component does not exist
        Component* get(const std::string& name);

        // Returns a reference to a specific component type
        // Note: Component is automatically created if it doesn't exist
        template <typename T>
        T& at();

        // Returns a base component reference from bound string name
        // Note: Component is automatically created if it doesn't exist
        Component& at(const std::string& name);

        // Same as at() above
        Component& operator[](const std::string& name);

        // Returns true if the entity has all of the specified component types
        // TODO: Make variadic
        template <typename T>
        bool has() const;

        // Returns true if the entity has all of the specified component types
        // TODO: Make variadic
        bool has(const std::string& name) const;

        // Returns number of component instances
        size_t numComponents() const;


        // Removing components ===============================================

        // Removes all specified component types
        // TODO: Make variadic
        template <typename T>
        void remove();

        // Removes all specified component types by name
        // TODO: Make variadic
        void remove(const std::string& name);


        // Entity information ================================================

        ID getId() const;

        const std::string& getName() const;

        const std::string& getType() const;

        void setName(const std::string& name);

        void destroy();

        operator bool() const;


        // Serialization =====================================================

        std::vector<std::string> serialize() const;

        void deserialize(const std::vector<std::string>& componentStrings);

    private:

        World& world;
        ID id;
};

}

#endif
