// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include <es/world.h>

namespace es
{

World World::prototypes;

Entity World::create(const std::string& name)
{
    return {core, core.create(name)};
}

Entity World::copy(const std::string& prototypeName, const std::string& name)
{
    return prototypes.get(prototypeName).clone(core, name);
}

Entity World::clone(const std::string& prototypeName, const std::string& name)
{
    return copy(prototypeName, name);
}

Entity World::operator[](const std::string& name)
{
    return {core, core[name]};
}

Entity World::operator()(const std::string& prototypeName, const std::string& name)
{
    auto ent = get(name);
    if (!ent)
        ent = copy(prototypeName, name);
    return ent;
}

Entity World::operator[](ID id)
{
    return {core, id};
}

Entity World::get(const std::string& name)
{
    return {core, core.get(name)};
}

Entity World::get(ID id)
{
    return {core, id};
}

void World::destroy(ID id)
{
    get(id).destroy();
}

void World::destroy(const std::string& name)
{
    get(name).destroy();
}

void World::clear()
{
    core.clear();
}

World::EntityList World::query()
{
    EntityList entities;
    for (auto id: core.entities.getIndex())
        entities.emplace_back(core, id);
    return entities;
}

bool World::valid(ID id) const
{
    return core.isValid(id);
}

bool World::valid(const std::string& name) const
{
    return core.isValid(core.get(name));
}

size_t World::size() const
{
    return core.entities.size();
}

World::operator Core&()
{
    return core;
}

bool World::validName(const std::string& compName)
{
    return ComponentPool::validName(compName);
}

}
