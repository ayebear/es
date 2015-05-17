// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef PACKEDARRAY_H
#define PACKEDARRAY_H

#include <cstdint>
#include <vector>
#include <limits>
#include "reversemap.h"

namespace es
{

/*
PackedArray 2.0
    100% unique IDs, never reused (until overflow)
        Just increments the ID each time something new is added
    Generic "handles", which are smart pointers that don't invalidate when reallocating memory
        The lookup is done when using the handle
        The handle stores the ID instead of a raw pointer
    O(1): Add, access, remove
    Supports directly iterating through internal array
*/
template <class Type>
class PackedArray
{
    public:

        using ID = uint64_t;
        static const ID invalidId = std::numeric_limits<ID>::max();
        using Container = std::vector<Type>;

        class Handle
        {
            public:
                Handle(PackedArray<Type>& array, ID id): array(array), id(id) {}
                bool valid() const { return array.isValid(id); }
                Type& access() { return array[id]; }
                operator Type&() { return access(); }
                Type* operator-> () { return &access(); }
                Type& operator* () { return access(); }

            private:
                PackedArray<Type>& array;
                PackedArray::ID id;
        };

        PackedArray() {}

        PackedArray(size_t spaceToReserve)
        {
            elements.reserve(spaceToReserve);
            index.getKeyMap().reserve(spaceToReserve);
            index.getValueMap().reserve(spaceToReserve);
        }

        // Adds a new object and returns its ID
        template <typename... Args>
        ID create(Args&&... args)
        {
            ID id = nextId++;
            index.insert(id, elements.size());
            elements.emplace_back(std::forward<Args>(args)...);
            return id;
        }

        // Returns a reference to the object with the specified ID
        Type& operator[] (ID id)
        {
            return elements[index.getValue(id)];
        }

        // Returns a handle to the object with the specified ID
        Handle get(ID id)
        {
            return Handle(*this, id);
        }

        // Returns true if the ID is valid
        bool isValid(ID id) const
        {
            return (index.getValue(id) != index.invalidValue);
        }

        // Removes the object with the specified ID
        void erase(ID id)
        {
            auto pos = index.getValue(id);
            if (pos != index.invalidValue)
            {
                // Overwrite the object being erased
                auto swappedPos = swapErase(pos);
                auto swappedId = index.getKey(swappedPos);

                // Remove the unused values
                index.getValueMap().erase(id);
                index.getKeyMap().erase(swappedPos);

                // Update both maps using their existing keys
                index.insert(swappedId, pos);
            }
        }

        // Clears all of the elements and IDs
        void clear()
        {
            elements.clear();
            index.clear();
        }

        // Returns the number of elements
        size_t size() const
        {
            return elements.size();
        }

        typename std::vector<Type>::iterator begin()
        {
            return elements.begin();
        }

        typename std::vector<Type>::iterator end()
        {
            return elements.end();
        }

        typename std::vector<Type>::const_iterator cbegin() const
        {
            return elements.cbegin();
        }

        typename std::vector<Type>::const_iterator cend() const
        {
            return elements.cend();
        }

    private:

        size_t swapErase(size_t pos)
        {
            // This function overwrites the specified element with the last element
            // Returns the position of the element that was moved, or invalid if nothing else was affected
            size_t oldPos = index.invalidValue;
            if (pos < elements.size() && !elements.empty())
            {
                if (elements.size() == 1)
                    elements.pop_back(); // Remove the last and only object
                else
                {
                    elements[pos] = std::move(elements.back()); // Replace the old object with the last one
                    elements.pop_back(); // Remove the last object
                    oldPos = elements.size(); // The ID of the old last object
                }
            }
            return oldPos;
        }

        ID nextId = 0;
        Container elements;
        ReverseMap<ID, size_t> index;
};

}

#endif
