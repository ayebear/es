// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "es/entityprototypeloader.h"
#include "es/world.h"

namespace es
{

bool loadPrototypes(const std::string& configFilename)
{
    EntityPrototypeLoader loader(configFilename);
    return loader.load();
}

EntityPrototypeLoader::EntityPrototypeLoader(const std::string& configFilename):
    config(configFilename)
{
}

bool EntityPrototypeLoader::load()
{
    // Load components from config file
    bool status = config.getStatus();
    if (status)
    {
        extractParentInfo();
        loadAllEntities();
    }
    return status;
}

void EntityPrototypeLoader::extractParentInfo()
{
    for (auto& section: config)
    {
        // Get inheritance information from section name
        std::string entityName;
        auto parentEntities = splitNames(section.first, entityName);

        // Store parent list and copy original component data
        entToComp[entityName].parentNames = std::move(parentEntities);
        entToComp[entityName].componentData = std::move(section.second);
    }
}

void EntityPrototypeLoader::loadAllEntities()
{
    for (auto& parentInfo: entToComp)
    {
        std::cout << "Loading entity '" << parentInfo.first << "'...\n";
        visitedParents.clear();
        loadEntity(parentInfo.first, parentInfo.first);
    }
}

std::vector<std::string> EntityPrototypeLoader::splitNames(const std::string& sectionName, std::string& entityName)
{
    auto outerSplit = strlib::split(sectionName, ":");

    // Set the entity name (whether it has parents or not)
    if (!outerSplit.empty())
        entityName = outerSplit.front();

    // Split and trim parent names (if there are any parents)
    std::vector<std::string> parentEntities;
    if (outerSplit.size() == 2)
    {
        parentEntities = strlib::split(outerSplit.back(), ",");
        for (auto& parentName: parentEntities)
            strlib::trimWhitespace(parentName);
    }
    return parentEntities;
}

void EntityPrototypeLoader::loadEntity(const std::string& entityName, const std::string& parentName)
{
    // Don't process a parent that doesn't exist
    if (entToComp.find(parentName) == entToComp.end())
    {
        std::cout << "  Error: '" << parentName << "' doesn't exist.\n";
        return;
    }

    // Mark this entity name as visited
    visitedParents.insert(parentName);

    // Loop through any parents, and call this function
    for (auto& parent: entToComp[parentName].parentNames)
    {
        // Make sure parents are never re-visited before recursing
        if (visitedParents.find(parent) == visitedParents.end())
            loadEntity(entityName, parent);
    }

    // Load the components of this entity or parent
    loadComponents(entityName, entToComp[parentName].componentData);
}

void EntityPrototypeLoader::loadComponents(const std::string& entityName, const cfg::File::Section& section)
{
    // Load each component from a string in the section
    for (auto& option: section)
        World::prototypes[entityName].deserialize(option.first, option.second.toString());
}

}
