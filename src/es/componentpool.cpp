// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "es/componentpool.h"

namespace es
{

const std::type_index ComponentPool::invalidTypeIdx {typeid(void)};

ComponentPool::ComponentPool()
{
    refresh();
}

bool ComponentPool::validName(const std::string& compName)
{
    return (getCompNames().find(compName) != getCompNames().end());
}

BaseComponentArray* ComponentPool::operator[](const std::type_index& typeIdx) const
{
    auto found = components.find(typeIdx);
    if (found != components.end())
        return found->second.get();
    return nullptr;
}

BaseComponentArray* ComponentPool::operator[](const std::string& compName)
{
    // Get the type index from the component name
    auto info = getCompNames().find(compName);
    if (info != getCompNames().end())
        return setupArray(info->second);
    return nullptr;
}

const std::type_index& ComponentPool::getTypeIndex(const std::string& compName) const
{
    // Get the type index from the component name
    auto info = getCompNames().find(compName);
    if (info != getCompNames().end())
        return info->second.typeIdx;
    return invalidTypeIdx;
}

void ComponentPool::reset()
{
    components.clear();
    refresh();
}

void ComponentPool::refresh()
{
    for (auto& info: getCompNames())
        setupArray(info.second);
}

BaseComponentArray* ComponentPool::setupArray(const ComponentInfo& info)
{
    // Clone the component array if one doesn't already exist for this type index
    auto& compArray = components[info.typeIdx];
    if (!compArray)
        compArray = info.compArray->clone();
    return compArray.get();
}

}
