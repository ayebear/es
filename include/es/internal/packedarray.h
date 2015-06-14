// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_PACKEDARRAY_H
#define ES_PACKEDARRAY_H

#include <cstdint>
#include <vector>
#include <limits>
#include "es/internal/id.h"
#include "es/internal/handle.h"

namespace es
{

const uint32_t u32Max = std::numeric_limits<uint32_t>::max();

/*
PackedArray 3.0
    O(1): Add, access, update, remove
    Unique IDs
        IDs have an index and a version
        The version is incremented each time the index is erased
    Free list stored inside holes of index
        No memory allocations when erasing elements
    Generic "handles", which are smart pointers that don't invalidate when reallocating memory
        The lookup is done when using the handle
        The handle stores the ID instead of a raw pointer
    Supports directly iterating through internal array
*/
template <class T>
class PackedArray
{
    public:

        using Container = std::vector<T>;

        PackedArray() {}

        PackedArray(size_t spaceToReserve)
        {
            index.reserve(spaceToReserve);
            elements.reserve(spaceToReserve);
            reverseLookup.reserve(spaceToReserve);
        }

        // Adds a new object and returns its ID
        template <typename... Args>
        ID create(Args&&... args)
        {
            uint32_t pos = elements.size();
            elements.emplace_back(std::forward<Args>(args)...);
            return addToIndex(pos);
        }

        // Returns a reference to the object with the specified ID
        // Warning: Using this with an invalid ID is undefined behavior
        T& operator[] (ID id)
        {
            // The cast gets the index from the ID, which is the same as PID{id}.index
            return elements[index[static_cast<uint32_t>(id)].index];
        }

        // Returns a const reference to the object with the specified ID
        // Warning: Using this with an invalid ID is undefined behavior
        const T& operator[] (ID id) const
        {
            // The cast gets the index from the ID, which is the same as PID{id}.index
            return elements[index[static_cast<uint32_t>(id)].index];
        }

        // Returns a pointer to an object, or nullptr if the ID is invalid
        T* get(ID id)
        {
            if (isValid(id))
                return &operator[](id);
            return nullptr;
        }

        // Returns a const pointer to an object, or nullptr if the ID is invalid
        const T* get(ID id) const
        {
            if (isValid(id))
                return &operator[](id);
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
            PID pid{id};
            return (pid.version &&
                    pid.index < index.size() &&
                    index[pid.index].used &&
                    index[pid.index].version == pid.version);
        }

        // Removes the object with the specified ID
        void erase(ID id)
        {
            if (isValid(id))
            {
                PID pid{id};
                uint32_t pos = index[pid.index].index;

                // Adds to free list, marks as unused, increments version
                removeFromIndex(pid.index);

                // Overwrite element with last element
                uint32_t swappedPos = swapErase(elements, pos);

                // Update index with new position if something was swapped
                if (swappedPos != u32Max)
                    index[reverseLookup[swappedPos]].index = pos;

                // Remove position from reverse lookup
                swapErase(reverseLookup, pos);
            }
        }

        // Clears all of the elements and IDs
        void clear()
        {
            head = u32Max;
            index.clear();
            elements.clear();
            reverseLookup.clear();
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

        // Returns all of the currently used IDs
        std::vector<ID> getIndex() const
        {
            std::vector<ID> ids;
            uint32_t count = 0;
            for (const auto& pid: index)
            {
                if (pid.used)
                    ids.push_back(PID{pid.version, count}.id());
                ++count;
            }
            return ids;
        }

    private:

        // Adds to free list, marks as unused, increments version
        // (Index position)
        void removeFromIndex(uint32_t pos)
        {
            // Increment version and mark as unused
            auto& idx = index[pos];
            idx.incVersion();
            idx.used = false;

            // Add to free list
            idx.index = head;
            head = pos;
        }

        // Updates the index to point to the specified (elements) position
        // Also updates the reverse lookup table
        ID addToIndex(uint32_t pos)
        {
            PID pid;
            if (head == u32Max)
            {
                // Index has no holes, so add ID to the end
                pid.index = index.size();
                index.emplace_back(1, pos);
            }
            else
            {
                // Reuse the first ID from the free list (pop it off)
                // Note: Version is different from previous erase
                auto& idx = index[head];
                pid.version = idx.version;
                pid.index = head;
                head = idx.index;
                idx.index = pos;
                idx.used = true;
            }
            reverseLookup.push_back(pid.index);
            return pid.id();
        }

        // This function overwrites the specified element with the last element
        // Returns the position of the element that was moved, or invalid if nothing else was affected
        template <typename A>
        uint32_t swapErase(std::vector<A>& elements, uint32_t pos)
        {
            uint32_t oldPos = u32Max;
            uint32_t elemSize = elements.size();
            if (elemSize && pos < elemSize)
            {
                // Remove the last object without swapping
                if (elemSize == 1 || pos == elemSize - 1)
                    elements.pop_back();
                else
                {
                    // Replace old element with last element
                    elements[pos] = std::move(elements.back());
                    elements.pop_back();
                    oldPos = elements.size();
                }
            }
            return oldPos;
        }

        // Position of first node of linked list, in the holes of the index
        uint32_t head {u32Max};

        // External position to internal position and version
        // Also contains the free list inside of the "holes"
        std::vector<PID> index;

        // Actual elements are stored here
        // This has no "holes", because it's "packed"
        Container elements;

        // Used to update index when elements are swapped
        // Parallel to elements vector
        std::vector<uint32_t> reverseLookup;
};

}

#endif
