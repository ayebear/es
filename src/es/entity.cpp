// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "es/entity.h"
#include <cassert>

namespace es
{

Entity& Entity::operator<<(const std::string& data)
{
    return deserialize(data);
}

Entity& Entity::operator<<(const char* data)
{
    return deserialize(data);
}

Handle<BaseComponentArray, Component> Entity::get(const std::string& name)
{
    return {core.components[name], getCompId(name)};
}

Component* Entity::getPtr(const std::string& name)
{
    if (valid())
    {
        auto compArray = core.components[name];
        if (compArray)
            return compArray->get(getCompId(name));
    }
    return nullptr;
}

std::vector<std::string> Entity::getNames() const
{
    std::vector<std::string> names;
    for (const auto& comp: core.entities[id].compSet)
    {
        // Get the component's name by type index
        auto name = core.components.getName(comp.first);
        if (!name.empty())
            names.push_back(name);
    }
    return names;
}

Handle<BaseComponentArray, Component> Entity::at(const std::string& name)
{
    return {core.components[name], atCompId(name)};
}

Component* Entity::accessPtr(const std::string& name)
{
    auto compArray = core.components[name];
    if (compArray)
        return compArray->get(atCompId(name));
    return nullptr;
}

Component& Entity::access(const std::string& name)
{
    auto comp = accessPtr(name);
    assert(comp);
    return *comp;
}

Component& Entity::operator[](const std::string& name)
{
    return access(name);
}

Component& Entity::operator[](const char* name)
{
    return access(name);
}

bool Entity::has(const std::string& name) const
{
    return (getCompId(name) != invalidId);
}

size_t Entity::total() const
{
    if (valid())
        return core.entities[id].compSet.size();
    return 0;
}

bool Entity::empty() const
{
    return total() == 0;
}

void Entity::remove(const std::string& name)
{
    removeComp(core.components.getTypeIndex(name));
}

void Entity::clear()
{
    if (valid())
    {
        // Go through component set, and remove components by ID
        auto& compSet = core.entities[id].compSet;
        for (const auto& comp: compSet)
            core.components[comp.first]->erase(comp.second);
        compSet.clear();
    }
}

Entity Entity::clone(const std::string& newName) const
{
    return clone(core, newName);
}

Entity Entity::clone(Core& newCore, const std::string& newName) const
{
    ID newId = invalidId;
    if (valid())
    {
        newId = newCore.create(newName);
        copyComponents(core, id, newCore, newId);
    }
    return Entity(newCore, newId);
}

ID Entity::getId() const
{
    return id;
}

const std::string& Entity::getName() const
{
    return core.getName(id);
}

void Entity::setName(const std::string& name)
{
    core.setName(id, name);
}

void Entity::invalidate()
{
    id = invalidId;
}

void Entity::destroy()
{
    clear();
    core.remove(id);
    invalidate();
}

bool Entity::valid() const
{
    return core.isValid(id);
}

Entity::operator bool() const
{
    return valid();
}

std::vector<std::string> Entity::serialize() const
{
    std::vector<std::string> comps;
    for (const auto& comp: core.entities[id].compSet)
    {
        // Get the component's name by type index
        auto compName = core.components.getName(comp.first);
        if (!compName.empty())
        {
            // Serialize the component with the name and data
            auto compData = (*core.components[comp.first])[comp.second].save();
            if (!compData.empty())
                compName += ' ' + compData;
            comps.push_back(compName);
        }
    }
    return comps;
}

Entity& Entity::deserialize(const std::string& compName, const std::string& compData)
{
    auto comp = at(compName);
    if (comp)
        comp->load(compData);
    return *this;
}

Entity& Entity::deserialize(const std::string& data)
{
    if (valid())
    {
        // To to find and split at the first space
        auto separator = data.find(' ');
        if (separator == std::string::npos)
            deserialize(data, "");
        else if (separator > 0 && separator + 1 < data.size())
        {
            // Extract the component's name and data
            auto compName = data.substr(0, separator);
            auto compData = data.substr(separator + 1);
            deserialize(compName, compData);
        }
    }
    return *this;
}

Entity& Entity::deserialize(const std::vector<std::string>& componentStrings)
{
    for (const auto& str: componentStrings)
        deserialize(str);
    return *this;
}

void Entity::copyComponents(const Core& srcCore, ID srcId, Core& destCore, ID destId) const
{
    // Loop through source entity's components, and copy each one
    auto& destCompSet = destCore.entities[destId].compSet;
    for (auto srcCompId: srcCore.entities[srcId].compSet)
    {
        auto destCompArray = destCore.components[srcCompId.first];
        assert(destCompArray);
        destCompSet[srcCompId.first] = destCompArray->copyFrom(*srcCore.components[srcCompId.first], srcCompId.second);
    }
}

ID Entity::getCompId(const std::string& name) const
{
    return getCompId(core.components.getTypeIndex(name));
}

ID Entity::getCompId(const std::type_index& typeIdx) const
{
    if (valid())
    {
        auto& compSet = core.entities[id].compSet;
        auto found = compSet.find(typeIdx);
        if (found != compSet.end())
            return found->second;
    }
    return invalidId;
}

ID Entity::atCompId(const std::string& name)
{
    ID compId = getCompId(name);
    if (compId == invalidId)
    {
        auto compArray = core.components[name];
        if (compArray)
        {
            compId = compArray->create();
            core.entities[id].compSet[core.components.getTypeIndex(name)] = compId;
        }
    }
    return compId;
}

void Entity::removeComp(const std::type_index& typeIdx)
{
    ID compId = getCompId(typeIdx);
    if (compId != invalidId)
    {
        // Erase actual component and ID from component set
        core.components[typeIdx]->erase(compId);
        core.entities[id].compSet.erase(typeIdx);
    }
}

}
