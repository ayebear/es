// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_PACKEDARRAY_H
#define ES_PACKEDARRAY_H

#include <cstdint>
#include <vector>
#include <limits>
#include "es/internal/id.h"
#include "es/internal/reversemap.h"
#include "es/internal/handle.h"

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
template <class T>
class PackedArray
{
    public:

        using Container = std::vector<T>;
        using Index = ReverseMap<ID, size_t>;

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
        // Warning: Using this with an invalid ID is undefined behavior
        T& operator[] (ID id)
        {
            return elements[index.getValue(id)];
        }

        // Returns a const reference to the object with the specified ID
        // Warning: Using this with an invalid ID is undefined behavior
        const T& operator[] (ID id) const
        {
            return elements[index.getValue(id)];
        }

        // Returns a pointer to an object, or nullptr if the ID is invalid
        T* get(ID id)
        {
            auto pos = index.getValue(id);
            if (pos != index.invalidValue)
                return &elements[pos];
            return nullptr;
        }

        // Returns a const pointer to an object, or nullptr if the ID is invalid
        const T* get(ID id) const
        {
            auto pos = index.getValue(id);
            if (pos != index.invalidValue)
                return &elements[pos];
            return nullptr;
        }

        // Returns a handle to the object with the specified ID
        Handle<PackedArray<T>, T> getHandle(ID id)
        {
            return {this, id};
        }

        // Returns a const handle to the object with the specified ID
        const Handle<const PackedArray<T>, const T> getHandle(ID id) const
        {
            return {this, id};
        }

        // Returns true if the ID is valid
        bool isValid(ID id) const
        {
            return (id != invalidId && index.getValue(id) != index.invalidValue);
        }

        // Removes the object with the specified ID
        void erase(ID id)
        {
            auto pos = index.getValue(id);
            if (pos != index.invalidValue)
            {
                // Overwrite the object being erased
                auto swappedPos = swapErase(pos);

                if (swappedPos == index.invalidValue)
                {
                    // Clear the index if there are no more elements
                    index.clear();
                }
                else if (swappedPos == pos)
                {
                    // Remove the ID/pos that was erased (nothing was swapped)
                    index.eraseByValue(pos);
                }
                else
                {
                    auto swappedId = index.getKey(swappedPos);

                    // Remove the unused values
                    index.getValueMap().erase(id);
                    index.getKeyMap().erase(swappedPos);

                    // Update both maps using their existing keys
                    index.insert(swappedId, pos);
                }
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

        typename Container::iterator begin()
        {
            return elements.begin();
        }

        typename Container::iterator end()
        {
            return elements.end();
        }

        typename Container::const_iterator cbegin() const
        {
            return elements.cbegin();
        }

        typename Container::const_iterator cend() const
        {
            return elements.cend();
        }

        const Index::ValueMap& getIndex() const
        {
            return index.getValueMap();
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
                    if (pos != elements.size() - 1)
                        elements[pos] = std::move(elements.back()); // Replace the old object with the last one
                    elements.pop_back(); // Remove the last object
                    oldPos = elements.size(); // The ID of the old last object
                }
            }
            return oldPos;
        }

        ID nextId = 0;
        Container elements;
        Index index;
};

}

#endif
