// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_ID_H
#define ES_ID_H

#include <cstdint>

namespace es
{

using ID = uint64_t;
static const ID invalidId = 0;

/*
Invalid ID:
    Version = 0
    Index = Anything

Elements:
    Used = true
    Version = 1 (+1 each delete)
    Index = Internal position of element

Linked list:
    Next node:
        Used = false
        Index = Position of next node

    End of list
        Used = false
        Index = Max
*/

// PackedArray ID
struct PID
{
    PID():
    	version(1),
    	index(0)
    {}

    // Extracts values from ID
    PID(ID id):
        version(id >> 32),
        index(id)
        // index(id & 0xFFFFFFFFUL)
    {}

    PID(uint32_t version, uint32_t index):
    	version(version),
    	index(index)
    {}

    // Combines values to an ID
    ID id() const
    {
        return ((static_cast<ID>(version) << 32) | static_cast<ID>(index));
    }

    // Handles wraparound, so version is always valid
    void incVersion()
    {
        if (!++version)
            version = 1;
    }

    bool used {true};
    uint32_t version;
    uint32_t index;
};

}

#endif
