// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_ENTITYPROTOTYPELOADER_H
#define ES_ENTITYPROTOTYPELOADER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include "configfile.h"

namespace es
{

// Loads all prototypes from a config file (returns true if successful)
bool loadPrototypes(const std::string& configFilename);

/*
This is an alternate version of the ObjectPrototypeLoader class from OCS.
It uses cfg::File formatted files instead, to keep things consistent with the rest of the project.
Inheritance:
    You can have an entity "inherit" from another entity, which means all of the
        parent's components will be used in addition to the child's components.
    Components from the child entity override the ones from the parent entity.
    The highest level parent's components are loaded first.
    Multiple inheritance is supported.
        The order the parents are listed is the order they are loaded.

Example file:

[SomeEntity]
Position = "200 200"
Velocity = "50 50"
Size = "128 128"

[AnotherEntity]
SomeComponent = "parameter1 parameter2 parameter3"
AnotherComponent = ""

[SubEntity: SomeEntity, AnotherEntity]
Description = "An entity with all of the components of SomeEntity and AnotherEntity"

[SubEntity2: SomeEntity]
Size = "64 64"

Example usage:

es::loadPrototypes("entities.cfg");

*/
class EntityPrototypeLoader
{
    public:
        EntityPrototypeLoader(const std::string& configFilename);
        bool load();

    private:

        // Parse inheritance lists and store the information in entToComp
        void extractParentInfo();

        // Loads all of the components for all of the entities
        void loadAllEntities();

        // Builds an inheritance list and extracts an entity name
        std::vector<std::string> splitNames(const std::string& sectionName, std::string& entityName);

        // Recursive function to load parent entity's components
        void loadEntity(const std::string& entityName, const std::string& parentName);

        // Loads all of the components from a section in the config file
        void loadComponents(const std::string& entityName, const cfg::File::Section& section);

        struct ParentInfo
        {
            std::vector<std::string> parentNames;
            cfg::File::Section componentData;
        };

        cfg::File config;
        std::map<std::string, ParentInfo> entToComp;

        // Temporary for each entity
        std::unordered_set<std::string> visitedParents;
};

}

#endif
