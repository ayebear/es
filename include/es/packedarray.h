// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef PACKEDARRAY_H
#define PACKEDARRAY_H

#include <vector>
#include <utility>
#include <ctype>

namespace es
{

/*
TODO:
    Unit tests for every possible scenario, including failing ones
    Implement free list as part of the holes in the outer index
    Consider using an unordered_map for the index
        IDs shouldn't be reused if possible as well
*/

/*
This class manages an array of objects in an efficient way, with good cache efficiency and minimal memory allocations.
Adding, removing, and accessing objects are all O(1) operations.
    Note: Except when more space needs to be allocated, which is O(n)
The internal order of the objects can rearrange when removing objects to keep everything contiguous.
The external IDs which are returned by the create function are guaranteed not to change unless
    erase is called with that ID, or if clear is called.

Here are some visual examples of how the packed array works.
Note that the outer array and free list are managed by the Index class.

Example packed array object:
            0   1   2   3   4   (this is how everything outside can access/erase elements)
Outer    : [ ] [2] [ ] [0] [1]  (array of ints, can have "holes" which are actually -1)
Free list: {0, 2}               (deque of ints, contains all of the free IDs)
Reverse  : [3] [4] [1]          (array of ints, parallel to the inner array)
Inner    : [x] [y] [z]          (array of the templated type, no "holes", this should be iterated through)

Accessing packedArray[1] would return z, packedArray[3] would return x, packedArray[4] would return y.
Accessing packedArray[0] or packedArray[2] would be undefined behavior as they are invalid IDs.

After erase(3):
Outer    : [ ] [0] [ ] [ ] [1]  // 3rd element gets set to -1
Free list: {0, 2, 3}            // 3 gets added to the free list
Reverse  : [1] [4]              // 0th element gets replaced with 2nd element
Inner    : [z] [y]              // 0th element gets replaced with 2nd element

After create(a):
Outer    : [2] [0] [ ] [ ] [1]  // 0th element gets internal ID set to 2
Free list: {2, 3}               // 0 gets removed from free list
Reverse  : [1] [4] [0]          // 0 gets added to reverse lookup table
Inner    : [z] [y] [a]          // a gets added to inner array

Client-server synchronization:
Server:
{0, 1, 2, 3, 4, 5, 6, 7, 8, 9} // Index
{a, b, c, d, e, f, g, h, i, j} // Internal objects
Client: (only is near a few)
{_, _, 0, 1, 2, _, 3, _, _, _} // Index
{c, d, e, g}                   // Internal objects
Note how the external IDs stay the same, without having to use yet another layer of indirection.
Also, the internal array is still packed, allowing for good cache efficiency.

Example usage:
PackedArray<string> names;
ID bobId = names.create("Bob");
cout << names[bobId] << endl; // Bob
ID joeId = names.create("Joe");
cout << names[joeId] << endl; // Joe
names.erase(bobId); // Removes Bob from the array
cout << names[joeId] << endl; // Still Joe, even though Joe has taken Bob's space in memory

// You can iterate through the array like a normal STL container:
names.create("Kevin");
names.create("Miles");
names.create("Eric");
// C++11:
for (auto& n: names)
    cout << n << endl;
// Or with iterators:
for (auto i = names.begin(); i != names.end(); ++i)
    cout << *i << endl;
// This is recommended when you need to access all of the elements in the array, as this will
// be good with your cache since it is accessing the internal contiguous array instead of
// looking up the IDs in the external index.
*/

template <class Type>
class PackedArray
{
    public:

        using ID = int64_t;

        PackedArray()
        {
        }

        // Pre-allocate an amount of elements
        PackedArray(size_t elementsToReserve)
        {
            elements.reserve(elementsToReserve);
            reverseLookup.reserve(elementsToReserve);
        }

        ////////////////////////////////////////////////////////////////////////////
        // The following functions access everything using the external index IDs //
        ////////////////////////////////////////////////////////////////////////////

        // Adds a new object and returns its ID
        template <typename... Args>
        ID create(Args&&... args)
        {
            ID internalId = elements.size();
            elements.emplace_back(std::forward<Args>(args)...);
            ID externalId = addToIndex(internalId);
            reverseLookup.push_back(externalId);
            return externalId;
        }

        // Returns a reference to the object with the specfied ID.
        // Warning: You must only pass valid IDs that were returned by the create function,
        // or this will cause undefined behavior. You can easily check if an ID is valid with
        // the isValid function.
        Type& operator[] (ID id)
        {
            return elements[index[id]];
        }

        // Returns true if the ID is in range and has an allocated space in the internal array
        bool isValid(ID id) const
        {
            return (id >= 0 && id < static_cast<ID>(index.size()) && index[id] >= 0);
        }

        // "Deallocates" an object by swapping and updating the index, so nothing appears to change
        void erase(ID id)
        {
            if (isValid(id)) // Make sure the ID is valid
            {
                ID internalId = index[id]; // Lookup the internal ID of the object
                removeFromIndex(id); // Remove the ID from the index
                ID swappedId = swapErase(elements, internalId); // Remove the object
                if (swappedId >= 0) // If another object was swapped
                    index[reverseLookup[swappedId]] = internalId; // Update the swapped object's internal ID
                swapErase(reverseLookup, internalId); // Remove the ID from the reverse lookup
                // (Needs to keep the array parallel with the elements array)
            }
        }

        // Clears all of the elements and IDs
        void clear()
        {
            elements.clear();
            reverseLookup.clear();
            index.clear();
            freeList.clear();
        }

        // Returns the current index size
        size_t size() const
        {
            return index.size();
        }

        ///////////////////////////////////////////////////////////////////////////
        // The following functions are for accessing the internal array directly //
        ///////////////////////////////////////////////////////////////////////////

        typename std::vector<Type>::iterator begin()
        {
            return elements.begin();
        }

        typename std::vector<Type>::iterator end()
        {
            return elements.end();
        }

        typename std::vector<Type>::const_iterator begin() const
        {
            return elements.cbegin();
        }

        typename std::vector<Type>::const_iterator end() const
        {
            return elements.cend();
        }

        // Returns the current number of allocated elements
        size_t capacity() const
        {
            return elements.size();
        }

    private:

        // Adds an internal ID to the index, returning the new external ID
        ID addToIndex(ID internalId)
        {
            ID newId;
            if (freeList.empty())
            {
                newId = index.size();
                index.push_back(internalId);
            }
            else
            {
                newId = freeList.back();
                freeList.pop_back();
                index[newId] = internalId;
            }
            return newId;
        }

        // Removes an external ID from the index
        void removeFromIndex(ID externalId)
        {
            freeList.push_back(index[externalId]);
            index[externalId] = -1;
        }

        template <typename Type>
        ID swapErase(std::vector<Type>& vec, size_t pos);

        std::vector<Type> elements; // Inner array containing the actual objects
        std::vector<ID> reverseLookup; // This is used for deleting elements and updating the proper IDs in the normal lookup
        std::vector<ID> index; // The index, which contains all of the internal IDs, and is accessed by external ID
        std::vector<ID> freeList; // The free "holes" in the index, used like a stack (used vector for clear function)
};

template <typename Type>
ID PackedArray::swapErase(std::vector<Type>& vec, size_t pos)
{
    // This function overwrites the specified element with the last element
    // Returns the position of the element that was moved, or -1 if nothing else was affected
    ID oldPos = -1;
    if (pos < vec.size() && !vec.empty())
    {
        if (vec.size() == 1)
            vec.pop_back(); // Remove the last and only object
        else
        {
            vec[pos] = vec.back(); // Replace the old object with the last one
            vec.pop_back(); // Remove the last object
            oldPos = vec.size(); // The ID of the old last object
        }
    }
    return oldPos;
}

}

#endif
