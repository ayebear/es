// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <iostream>
#include <typeinfo>

namespace es
{

/*
Base component class. All components must inherit from this.
Note: CRTP is not used.
*/
struct Component
{
    Component() {}
    virtual ~Component() {}

    virtual std::string toString() const
    {
        return "";
    };

    virtual void fromString(const std::string& str) {};

    void operator=(const std::string& str)
    {
        fromString(str);
    };

    void operator=(const char* str)
    {
        fromString(str);
    };

    operator std::string() const
    {
        return toString();
    }
};

}

#endif
