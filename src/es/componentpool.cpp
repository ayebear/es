// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include <es/componentpool.h>

namespace es
{

ComponentPool::ComponentPool():
    data(getStaticData())
{
    data.instances.insert(this);
    refresh();
}

ComponentPool::~ComponentPool()
{
    data.instances.erase(this);
}

bool ComponentPool::validName(const std::string& compName)
{
    return (getStaticData().compTypes.find(compName) != getStaticData().compTypes.end());
}

const std::type_index& ComponentPool::getTypeIndex(const std::string& compName)
{
    // Get the type index from the component name
    auto found = getStaticData().compTypes.find(compName);
    if (found != getStaticData().compTypes.end())
        return found->second.id;
    return getStaticData().invalidType.id;
}

const std::string& ComponentPool::getName(const std::type_index& typeIdx)
{
    // Get the component name from the type index
    static const std::string emptyStr;
    auto info = getStaticData().compInfo.find(typeIdx);
    if (info != getStaticData().compInfo.end())
        return info->second.name;
    return emptyStr;
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
    return operator[](getTypeIndex(compName));
}

const BaseComponentArray* ComponentPool::operator[](const std::string& compName) const
{
    return operator[](getTypeIndex(compName));
}

void ComponentPool::reset()
{
    components.clear();
    refresh();
}

void ComponentPool::refresh()
{
    for (auto& info: data.compInfo)
        setupArray(info.first, info.second.array.get());
}

BaseComponentArray* ComponentPool::setupArray(const std::type_index& typeIdx, BaseComponentArray* array)
{
    // Clone the component array if one doesn't already exist for this type index
    auto& compArray = components[typeIdx];
    if (!compArray)
        compArray = array->clone();
    return compArray.get();
}

ComponentPool::StaticData& ComponentPool::getStaticData()
{
    static StaticData data;
    return data;
}

}
