// Copyright (C) 2015-2016 Eric Hebert (ayebear)
// This code is licensed under MIT, see LICENSE.txt for details.

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

Entity World::from(const Component& comp)
{
    return {core, comp.getOwnerId()};
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

void World::getTypeIndexString(std::vector<TypeIndex>& types, const std::string& name) const
{
    types.emplace_back(ComponentPool::getTypeIndex(name));
}

World::EntityList World::queryTypes(std::vector<TypeIndex>& types)
{
    // Compute minimum component array size
    size_t minSize = std::numeric_limits<size_t>::max();
    size_t minIndex = 0;
    size_t index = 0;
    for (auto& typeIdx: types)
    {
        auto compArray = core.components[typeIdx.id];
        assert(compArray);
        size_t size = compArray->size();
        if (size < minSize)
        {
            minSize = size;
            minIndex = index;
        }
        ++index;
    }

    // Get minimum type
    auto minType = types[minIndex].id;

    // Swap-erase the minimum type index from the vector
    if (types.size() >= 2 && minIndex != types.size() - 1)
        std::swap(types[minIndex], types.back());
    types.pop_back();

    // Loop through component array of the minimum size
    return iterate(minType, types);
}

World::EntityList World::iterate(const std::type_index& minType, std::vector<TypeIndex>& types)
{
    EntityList entities;
    auto compArray = core.components[minType];
    assert(compArray);
    for (size_t i = 0; i < compArray->size(); ++i)
    {
        // Get owner ID of component, in order to lookup entity
        es::ID ownerId = compArray->getElement(i).getOwnerId();
        auto ent = get(ownerId);

        // Add entity to list if it has all of the component types
        if (ent.has(types))
            entities.push_back(ent);
    }
    return entities;
}

}
