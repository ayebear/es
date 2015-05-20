// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef COMPONENTPOOL_H
#define COMPONENTPOOL_H

#include "es/componentarray.h"
#include <typeindex>
#include <iostream>

namespace es
{

/*
Each instance of ComponentPool stores its own components.
    Each component type is stored in a separate PackedArray.
    Base component arrays can be accessed by their string name.
Components must be properly registered first with esRegister().
    This only applies to accessing arrays with component names.
    If new components are registered AFTER a component pool is created,
        and you wish to use these components by name, call refresh().
    You can also create a component by type, which will create the array.
        This would allow you to use strings as well.
*/
class ComponentPool
{
    public:

        using ComponentArrayPtr = std::unique_ptr<BaseComponentArray>;

        ComponentPool();

        // Registers a new component type and it's name
        template <typename T>
        static void registerComponent(const std::string& compName)
        {
            auto& info = getCompNames()[compName];
            info.typeIdx = typeid(T);
            info.compArray = std::make_unique<ComponentArray<T>>();
            std::cout << "ComponentPool: Registered '" << compName << "'\n";
        }

        // Returns the component array from the component's type
        // Creates a new array if it doesn't exist
        template <typename T>
        ComponentArray<T>& get()
        {
            auto& compArray = components[typeid(T)];
            if (!compArray)
            {
                compArray = std::make_unique<ComponentArray<T>>();
                std::cout << "ComponentPool: Initialized new component array for unregistered type.\n";
            }
            return *static_cast<ComponentArray<T>*>(compArray.get());
        }

        // Returns the base component array from the component's name
        // Returns nullptr if it doesn't exist
        BaseComponentArray* operator[](const std::string& compName);

        // Clears all components, and sets up new arrays cloned from registered types
        void reset();

        // Adds any newly registered component types
        void refresh();

    private:

        std::unordered_map<std::type_index, ComponentArrayPtr> components;

        struct ComponentInfo
        {
            std::type_index typeIdx {typeid(void)};
            ComponentArrayPtr compArray; // Empty array, is cloned by ComponentPool instances
        };

        using ComponentInfoMap = std::unordered_map<std::string, ComponentInfo>;

        static ComponentInfoMap& getCompNames()
        {
            static ComponentInfoMap componentNames;
            return componentNames;
        }
};

}

#endif
