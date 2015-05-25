// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_COMPONENTPOOL_H
#define ES_COMPONENTPOOL_H

#include "es/componentarray.h"
#include <typeindex>
#include <iostream>

namespace es
{

/*
Each instance of ComponentPool stores its own components.
    Each component type is stored in a separate PackedArray.
    Base component arrays can be accessed by their string name.
In order to use component names, you must register the name and
    type with registerComponent() or the esRegister() macro.
*/
class ComponentPool
{
    public:

        ComponentPool();

        // Registers a new component type and it's name
        template <typename T>
        static void registerComponent(const std::string& compName);

        // Returns true if the component name is valid
        static bool validName(const std::string& compName);

        // Returns the component array from the component's type
        // Creates a new array if it doesn't exist
        template <typename T>
        ComponentArray<T>& get();

        // Returns the base componeny array from the component's type index
        BaseComponentArray* operator[](const std::type_index& typeIdx) const;

        // Returns the base component array from the component's name
        // Creates a new array if it doesn't exist
        // Returns nullptr if the type is invalid
        BaseComponentArray* operator[](const std::string& compName);

        // Returns the type index of a component from string name
        const std::type_index& getTypeIndex(const std::string& compName) const;

        // Clears all components, and sets up new arrays cloned from registered types
        void reset();

        // Adds any newly registered component types
        void refresh();

    private:

        using ComponentArrayPtr = std::unique_ptr<BaseComponentArray>;

        // All components are stored here, separated by type
        std::unordered_map<std::type_index, ComponentArrayPtr> components;

        static const std::type_index invalidTypeIdx;

        // Contains a type index and a clonable empty component array
        struct ComponentInfo
        {
            std::type_index typeIdx {typeid(void)};
            ComponentArrayPtr compArray;
        };

        // Used for looking up types by component name
        using ComponentInfoMap = std::unordered_map<std::string, ComponentInfo>;
        static ComponentInfoMap& getCompNames()
        {
            static ComponentInfoMap componentNames;
            return componentNames;
        }

        // Refreshes a single array by type index and returns its pointer
        BaseComponentArray* setupArray(const ComponentInfo& info);
};

template <typename T>
void ComponentPool::registerComponent(const std::string& compName)
{
    auto& info = getCompNames()[compName];
    info.typeIdx = typeid(T);
    info.compArray = std::make_unique<ComponentArray<T>>();
    std::cout << "ComponentPool: Registered '" << compName << "'\n";
}

template <typename T>
ComponentArray<T>& ComponentPool::get()
{
    // Creates a new component array if necessary, and returns it
    auto& compArray = components[typeid(T)];
    if (!compArray)
        compArray = std::make_unique<ComponentArray<T>>();
    return *static_cast<ComponentArray<T>*>(compArray.get());
}

}

#endif
