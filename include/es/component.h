// Copyright (C) 2015-2016 Eric Hebert (ayebear)
// This code is licensed under MIT, see LICENSE.txt for details.

#ifndef ES_COMPONENT_H
#define ES_COMPONENT_H

#include <string>
#include <es/internal/id.h>

namespace es
{

class Entity;

/*
Base component class. All components must inherit from this.
*/
class Component
{
    public:

        Component() {}

        virtual ~Component() {}

        // Serializes the component to a string
        virtual std::string save() const
        {
            return {};
        }

        // Deserializes a string, and updates the component
        virtual void load(const std::string& str) {}

        // Deserializes from a string
        void operator=(const std::string& str)
        {
            load(str);
        }

        // Deserializes from a C string
        void operator=(const char* str)
        {
            load(str);
        }

        // Implicit conversion to a string
        operator std::string() const
        {
            return save();
        }

        // Returns the owner ID
        es::ID getOwnerId() const
        {
            return ownerId;
        }

    private:

        // The ID of the entity which contains this component
        es::ID ownerId{es::invalidId};

        friend class Entity;
};

}

#endif
