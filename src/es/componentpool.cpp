// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "es/componentpool.h"

namespace es
{

ComponentPool::ComponentPool()
{
    refresh();
}

BaseComponentArray* ComponentPool::operator[](const std::string& compName)
{
    // Get the type index from the component name
    auto info = getCompNames().find(compName);
    if (info != getCompNames().end())
        return setupArray(info->second);
    return nullptr;
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
