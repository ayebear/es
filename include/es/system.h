// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_SYSTEM_H
#define ES_SYSTEM_H

#include <es/world.h>

namespace es
{

/*
Base class for all systems.
Derived classes can be used with the SystemContainer class.
*/
class System
{
    public:
        System() {}
        virtual ~System() {}

        virtual void initialize() {}

        // Derived classes must implement this function
        virtual void update(float dt) = 0;

        void setDefaultWorld(es::World* world) { this->world = world;}
    protected:
       es::World* world{ nullptr };
};

}

#endif
