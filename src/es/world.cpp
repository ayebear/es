// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "es/world.h"

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

Entity World::operator[](const std::string& name)
{
    return {core, core[name]};
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

World::operator Core&()
{
    return core;
}

}
