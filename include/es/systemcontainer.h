// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_SYSTEMCONTAINER_H
#define ES_SYSTEMCONTAINER_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include "system.h"

namespace es
{

/*
This class can contain different systems, which are derived from the System base class.
It updates the systems in the order they were added, but also supports updating a single system by type.

Example usage:
    SystemContainer systems;
    systems.add<PhysicsSystem>(params);
    systems.add<RenderSystem>(window, params);
    systems.initializeAll();

    // In your game loop
    systems.updateAll(dt);

    // Also supports updating single systems by type:
    systems.update<RenderSystem>(dt);
*/
class SystemContainer
{
    public:
        SystemContainer();

        // Adds a new system. Usage:
        //     addSystem<SystemType>(constructor arguments);
        template <typename T, typename... Args>
        void add(Args&&... args);

        // Calls initialize() on all systems
        void initializeAll();

        // Initializes a specific system
        template <typename T>
        void initialize();

        // Calls update() on all systems
        // Note: The order this is called is the same order the systems were added
        void updateAll(float dt);

        // Updates a specific system
        template <typename T>
        void update(float dt);

    private:
        using SystemPtr = std::unique_ptr<System>;
        std::vector<SystemPtr> systems;
        std::unordered_map<std::type_index, size_t> systemTypes;

        // Returns a system pointer from the type (nullptr if it doesn't exist)
        template <typename T>
        System* getSystem();

        // Adds the type to the types table, and returns the index
        size_t getIndex(const std::type_index& type);
};

template <typename T, typename... Args>
void SystemContainer::add(Args&&... args)
{
    size_t index = getIndex(typeid(T));
    systems[index] = std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
void SystemContainer::initialize()
{
    auto system = getSystem<T>();
    if (system)
        system->initialize();
}

template <typename T>
void SystemContainer::update(float dt)
{
    auto system = getSystem<T>();
    if (system)
        system->update(dt);
}

template <typename T>
System* SystemContainer::getSystem()
{
    auto found = systemTypes.find(typeid(T));
    if (found != systemTypes.end())
        return systems[found->second].get();
    return nullptr;
}

}

#endif
