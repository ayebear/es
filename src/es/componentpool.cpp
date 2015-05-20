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
	// Get the type index from the string name
    auto typeIdx = getCompNames().find(compName);
    if (typeIdx != getCompNames().end())
    {
    	// Get the array pointer from the type index
        auto found = components.find(typeIdx->second.typeIdx);
        if (found != components.end())
            return found->second.get();
    }
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
	{
		// Clone the component array if one doesn't already exist for this type
		auto& compArray = components[info.second.typeIdx];
		if (!compArray)
			compArray = info.second.compArray->clone();
	}
}

}
