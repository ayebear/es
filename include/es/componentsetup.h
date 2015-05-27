// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_COMPONENTSETUP_H
#define ES_COMPONENTSETUP_H

#include "es/componentpool.h"

#define esRegister(compName) static es::RegisterName<compName> esCompReg_##compName(#compName);

namespace es
{

// Creating an instance of this will register the specified component type and name
template <class T>
struct RegisterName
{
    RegisterName(const char* name)
    {
        ComponentPool::registerComponent<T>(name);
    }
};

template<typename T>
void registerComponents()
{
	ComponentPool::registerComponent<T>(T::name);
}

template<typename A, typename B, typename... Args>
void registerComponents()
{
	registerComponents<A>();
	registerComponents<B, Args...>();
}

}

#endif
