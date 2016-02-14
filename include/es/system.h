// Copyright (C) 2015-2016 Eric Hebert (ayebear)
// This code is licensed under MIT, see LICENSE.txt for details.

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

        void setWorld(World* w)
        {
            world = w;
        }

        virtual void initialize() {}

        // Derived classes must implement this function
        virtual void update(float dt) = 0;

    protected:
        World* world{nullptr};
};

}

#endif
