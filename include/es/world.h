// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_WORLD_H
#define ES_WORLD_H

#include "es/internal/core.h"
#include "es/entity.h"

namespace es
{

/*
A wrapper class around Core and Entity.
Creates instances of Entity by constructing it with ID and Core&.
*/
class World
{
    public:

        World() {}


        // Creating entities =================================================

        // Creates an empty entity and returns it
        Entity create(const std::string& name = "");

        // Creates an entity with the specified components
        // TODO: Determine if this should deal with components

        // Creates an entity from a prototype
        Entity copy(const std::string& prototypeName, const std::string& name = "");


        // Creates a new entity if needed ====================================

        // Get entity by name
        Entity operator[](const std::string& name);


        // Returns an invalid Entity if it doesn't exist =====================

        // Get entity by ID
        Entity operator[](ID id);

        // Get entity by name
        Entity get(const std::string& name);

        // Get entity by ID
        Entity get(ID id);


        // Remove entities ===================================================

        // Removes all entities
        void clear();


        // Query entities ====================================================

        // EntityList query(const std::string& compName);

        // TODO: Add EntityList struct
        // TODO: Add variadic queries by name and type


        // Iterate through all entities ======================================

        // TODO: Add begin/end and const versions
            // This should iterate through the entities safely and return an
            // Entity handle each time.
        // Might need to add getElements() to packed array for this to work.


        // Miscellaneous =====================================================

        operator Core&();

        // Returns true if the component name is valid
        static bool validName(const std::string& compName);

        static World prototypes;

    private:

        Core core;

};

}

#endif
