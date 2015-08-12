// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include <es/systemcontainer.h>

namespace es
{

SystemContainer::SystemContainer()
{
}

void SystemContainer::initializeAll()
{
    // Call initialize on all of the systems
    for (auto& s: systems)
        s->initialize();
}

void SystemContainer::updateAll(float dt)
{
    // Call update on all of the systems
    for (auto& s: systems)
        s->update(dt);
}

size_t SystemContainer::getIndex(const std::type_index& type)
{
    auto found = systemTypes.find(type);
    if (found == systemTypes.end())
    {
        // Add the system pointer and store the index
        size_t index = systems.size();
        systems.emplace_back();
        systemTypes[type] = index;
        return index;
    }
    return found->second;
}

}
