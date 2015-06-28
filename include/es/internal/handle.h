// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_HANDLE_H
#define ES_HANDLE_H

#include "es/internal/id.h"

namespace es
{

// A smart handle that can access an element from a PackedArray-like container
template <class Container, class T>
class Handle
{
    public:
        Handle(Container* array, ID id): array(array), id(id) {}

        // Check if handle is valid
        bool valid() const { return array && array->isValid(id); }
        operator bool() const { return valid(); }

        // Erase element that handle is pointing to
        void erase() { if (array) array->erase(id); }

        // Dereference handle
        T& access() { return (*array)[id]; }
        const T& access() const { return (*array)[id]; }
        T* operator-> () { return &((*array)[id]); }
        T& operator* () { return (*array)[id]; }

        // Return pointer to element
        T* get() { return array ? array->get(id) : nullptr; }
        const T* get() const { return array ? array->get(id) : nullptr; }

    private:
        Container* array;
        ID id;
};

}

#endif
