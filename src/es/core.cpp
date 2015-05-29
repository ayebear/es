// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "es/internal/core.h"
#include <cassert>

namespace es
{

ID Core::create(const std::string& name)
{
    ID id = entities.create(name);
    if (!name.empty())
        entityNames[name] = id;
    return id;
}

ID Core::operator[](const std::string& name)
{
    auto found = entityNames.find(name);
    if (found == entityNames.end())
        return create(name);
    return found->second;
}

ID Core::get(const std::string& name) const
{
    auto found = entityNames.find(name);
    if (found != entityNames.end())
        return found->second;
    return invalidId;
}

bool Core::isValid(ID id) const
{
    return entities.isValid(id);
}

void Core::remove(ID id)
{
    if (isValid(id))
    {
        // Remove entity and name
        entityNames.erase(entities[id].name);
        entities.erase(id);
    }
}

void Core::clear()
{
    entities.clear();
    entityNames.clear();
    components.reset();
}

void Core::setName(ID id, const std::string& name)
{
    if (isValid(id))
    {
        // Deletes the old name from the lookup table before setting the new one
        auto& entName = entities[id].name;
        entityNames.erase(entName);
        entityNames[name] = id;
        entName = name;
    }
}

const std::string& Core::getName(ID id) const
{
    const static std::string noName("");
    if (isValid(id))
        return entities[id].name;
    return noName;
}

}
