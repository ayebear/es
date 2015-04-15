// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ENTITYPROTOTYPELOADER_H
#define ENTITYPROTOTYPELOADER_H

#include "OCS/Objects/ObjectManager.hpp"
#include "configfile.h"
#include <vector>

namespace es
{

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

Sample format:

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

*/
class EntityPrototypeLoader
{
    public:
        EntityPrototypeLoader(ocs::ObjectManager& objects, const std::string& configFilename);
        bool load();

        // Loads all prototypes from a config file (returns true if successful)
        static bool load(ocs::ObjectManager& objects, const std::string& configFilename);

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

        ocs::ObjectManager& objects;
        cfg::File config;
        std::map<std::string, ParentInfo> entToComp;

        // Temporary for each entity
        std::set<std::string> visitedParents;
};

}

#endif
