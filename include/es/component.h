// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_COMPONENT_H
#define ES_COMPONENT_H

#include <string>

namespace es
{

/*
Base component class. All components must inherit from this.
*/
struct Component
{
    Component() {}
    virtual ~Component() {}

    // Serializes the component to a string
    virtual std::string save() const
    {
        return {};
    }

    // Deserializes a string, and updates the component
    virtual void load(const std::string& str) {}

    void operator=(const std::string& str)
    {
        load(str);
    }

    void operator=(const char* str)
    {
        load(str);
    }

    operator std::string() const
    {
        return save();
    }
};

}

#endif
