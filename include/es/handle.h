// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_HANDLE_H
#define ES_HANDLE_H

namespace es
{

// A smart handle that can access an element from a PackedArray-like container
template <class Container, class T>
class Handle
{
    public:
        Handle(Container& array, ID id): array(array), id(id) {}
        bool isValid() const { return array.isValid(id); }
        operator bool() const { return isValid(); }
        void erase() { array.erase(id); }

        // Dereference handle
        T& access() { return array[id]; }
        T* operator-> () { return &array[id]; }
        T& operator* () { return array[id]; }

    private:
        Container& array;
        ID id;
};

}

#endif
