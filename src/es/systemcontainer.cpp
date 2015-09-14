// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include <es/systemcontainer.h>

namespace es
{

SystemContainer::SystemContainer()
{
}

SystemContainer::SystemContainer(World& w):
    world(&w)
{
}

void SystemContainer::setWorld(World& w)
{
    world = &w;
}

void SystemContainer::initializeAll()
{
    // Call initialize on all of the systems
    for (auto& s: systems)
        s.ptr->initialize();
}

void SystemContainer::updateAll(float dt)
{
    // Call update on all of the systems
    for (auto& s: systems)
        s.ptr->update(dt);
}

void SystemContainer::clear()
{
    systemTypes.clear();
    systems.clear();
}

size_t SystemContainer::size() const
{
    return systems.size();
}

size_t SystemContainer::getIndex(const std::type_index& type) const
{
    auto found = systemTypes.find(type);
    if (found != systemTypes.end())
        return found->second;
    return invalidIndex;
}

void SystemContainer::updateSystemTypes(size_t start)
{
    for (size_t i = start; i < systems.size(); ++i)
        systemTypes[systems[i].typeIndex] = i;
}

}
