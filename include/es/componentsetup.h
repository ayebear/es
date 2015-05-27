// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_COMPONENTSETUP_H
#define ES_COMPONENTSETUP_H

#include "es/componentpool.h"

namespace es
{

// Registers a single component type
template<typename T>
void registerComponents()
{
	ComponentPool::registerComponent<T>(T::name);
}

// Registers multiple component types
template<typename A, typename B, typename... Args>
void registerComponents()
{
	registerComponents<A>();
	registerComponents<B, Args...>();
}

}

#endif
