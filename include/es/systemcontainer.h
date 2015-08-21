// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_SYSTEMCONTAINER_H
#define ES_SYSTEMCONTAINER_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <limits>
#include <iostream>
#include <es/system.h>

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

        static const size_t invalidIndex = std::numeric_limits<size_t>::max();

        SystemContainer();

        // Adds a new system to the end of the list, and returns its index
        template <typename T, typename... Args>
        size_t add(Args&&... args);

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

        // Removes a specific system
        template <typename T>
        void remove();

        // Removes all systems
        void clear();

        // Swaps the order of two systems by type
        template <typename A, typename B>
        void swap();

        // Moves a system to a new index, shifting the current system at this index to the right
        // Note: 0 is the beginning, >= size() is the end
        template <typename T>
        void move(size_t destIndex);

        // Returns number of systems currently in container
        size_t size() const;

        // Returns position of system in container, or invalidIndex if it doesn't exist
        template <typename T>
        size_t getIndex() const;

        // Returns true if the system exists
        template <typename T>
        bool exists() const;

        // Returns a pointer to a system (nullptr if it doesn't exist)
        template <typename T>
        T* getSystem();

    private:

        struct SystemPtr
        {
            SystemPtr() {}
            SystemPtr(std::unique_ptr<System> ptr, const std::type_index& typeIndex):
                ptr(std::move(ptr)),
                typeIndex(typeIndex)
            {}
            std::unique_ptr<System> ptr;
            std::type_index typeIndex{typeid(void)};
        };

        std::vector<SystemPtr> systems;
        std::unordered_map<std::type_index, size_t> systemTypes;

        // Returns the position of a system by type index
        size_t getIndex(const std::type_index& type) const;

        // Rebuilds the system types index (default will rebuild all)
        void updateSystemTypes(size_t start = 0);
};

template <typename T, typename... Args>
size_t SystemContainer::add(Args&&... args)
{
    size_t index = invalidIndex;
    std::type_index typeIndex{typeid(T)};
    if (systemTypes.find(typeIndex) == systemTypes.end())
    {
        // Add the system pointer and store the index
        index = systems.size();
        systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...), typeIndex);
        systemTypes[typeIndex] = index;
    }
    else
        std::cout << "SystemContainer: Warning, '" << typeIndex.name() << "' was already added.\n";

    return index;
}

template <typename T>
void SystemContainer::initialize()
{
    auto sys = getSystem<T>();
    if (sys)
        sys->initialize();
}

template <typename T>
void SystemContainer::update(float dt)
{
    auto sys = getSystem<T>();
    if (sys)
        sys->update(dt);
}

template <typename T>
void SystemContainer::remove()
{
    size_t index = getIndex<T>();
    if (index != invalidIndex)
    {
        systems.erase(systems.begin() + index);
        systemTypes.erase(typeid(T));
        updateSystemTypes(index);
    }
}

template <typename A, typename B>
void SystemContainer::swap()
{
    auto indexA = getIndex<A>();
    auto indexB = getIndex<B>();
    systemTypes[typeid(A)] = indexB;
    systemTypes[typeid(B)] = indexA;
    std::swap(systems[indexA], systems[indexB]);
}

template <typename T>
void SystemContainer::move(size_t destIndex)
{
    size_t index = getIndex<T>();
    if (index != invalidIndex)
    {
        // Move out pointer and type
        SystemPtr systemPointer;
        systemPointer.ptr = std::move(systems[index].ptr);
        systemPointer.typeIndex = systems[index].typeIndex;

        // Erase original pointer location
        systems.erase(systems.begin() + index);

        // Re-insert back into vector
        auto iter = systems.end();
        if (destIndex < systems.size())
            iter = systems.begin() + destIndex;
        systems.insert(iter, std::move(systemPointer));

        // Update system types index
        updateSystemTypes(std::min(index, destIndex));
    }
}

template <typename T>
size_t SystemContainer::getIndex() const
{
    return getIndex(typeid(T));
}

template <typename T>
bool SystemContainer::exists() const
{
    return (getIndex<T>() != invalidIndex);
}

template <typename T>
T* SystemContainer::getSystem()
{
    auto found = systemTypes.find(typeid(T));
    if (found != systemTypes.end())
        return static_cast<T*>(systems[found->second].ptr.get());
    return nullptr;
}

}

#endif
