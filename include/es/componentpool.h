// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_COMPONENTPOOL_H
#define ES_COMPONENTPOOL_H

#include <typeindex>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <es/internal/componentarray.h>

namespace es
{

// A default-constructible type_index
struct TypeIndex
{
    TypeIndex(): id{typeid(void)} {}
    TypeIndex(const std::type_index& id): id{id} {}

    std::type_index id;
};

/*
Each instance of ComponentPool stores its own components.
    Each component type is stored in a separate PackedArray.
    Base component arrays can be accessed by component name or type index.
*/
class ComponentPool
{
    public:

        ComponentPool();
        ~ComponentPool();

        // Registers a new component type and it's name
        template <typename T>
        static void registerComponent(const std::string& compName = "");

        // Returns true if the component name is valid
        static bool validName(const std::string& compName);

        // Returns the type index of a component by name
        static const std::type_index& getTypeIndex(const std::string& compName);

        // Returns the name of a component by type index
        static const std::string& getName(const std::type_index& typeIdx);

        // Returns the component array from the component's type
        template <typename T>
        ComponentArray<T>& get();

        // Returns the component array from the component's type
        template <typename T>
        const ComponentArray<T>& get() const;

        // Returns the base componeny array from the component's type index
        BaseComponentArray* operator[](const std::type_index& typeIdx) const;

        // Returns the base component array from the component's name
        BaseComponentArray* operator[](const std::string& compName);

        // Returns the base component array from the component's name
        const BaseComponentArray* operator[](const std::string& compName) const;

        // Clears all components, and sets up new arrays cloned from registered types
        void reset();

        // Adds any newly registered component types
        void refresh();

    private:

        // For storing components
        using ComponentArrayPtr = std::unique_ptr<BaseComponentArray>;

        // Contains a clonable empty component array and the component name
        struct ComponentInfo
        {
            ComponentArrayPtr array;
            std::string name;
        };

        using TypeToInfoMap = std::unordered_map<std::type_index, ComponentInfo>;
        using NameToTypeMap = std::unordered_map<std::string, TypeIndex>;

        struct StaticData
        {
            // For keeping track of ComponentPool instances
            std::unordered_set<ComponentPool*> instances;

            TypeToInfoMap compInfo;
            NameToTypeMap compTypes;
            const TypeIndex invalidType;
        };

        // Refreshes a single array by type index and returns its pointer
        BaseComponentArray* setupArray(const std::type_index& typeIdx, BaseComponentArray* array);

        static StaticData& getStaticData();

        // Used in the initializer list to make sure the static variables get
        // initialized before any component pools.
        StaticData& data;

        // All components are stored here, separated by type
        std::unordered_map<std::type_index, ComponentArrayPtr> components;
};

template <typename T>
void ComponentPool::registerComponent(const std::string& compName)
{
    std::type_index typeIdx {typeid(T)};
    auto& info = getStaticData().compInfo[typeIdx];
    if (!info.array)
    {
        // Store the name -> type index
        if (!compName.empty())
            getStaticData().compTypes[compName].id = typeIdx;

        // Create an empty array, and save the component name
        info.array = std::make_unique<ComponentArray<T>>();
        info.name = compName;

        // Update current ComponentPool instances with new type
        for (auto ptr: getStaticData().instances)
            ptr->setupArray(typeIdx, info.array.get());
    }
}

template <typename T>
ComponentArray<T>& ComponentPool::get()
{
    // Only returns the array for registered types
    auto found = components.find(typeid(T));
    assert(found != components.end());
    return *static_cast<ComponentArray<T>*>(found->second.get());
}

template <typename T>
const ComponentArray<T>& ComponentPool::get() const
{
    // Only returns the array for registered types
    auto found = components.find(typeid(T));
    assert(found != components.end());
    return *static_cast<const ComponentArray<T>*>(found->second.get());
}

// Registers a single component type
template<typename T>
void registerComponents()
{
    ComponentPool::registerComponent<T>(T::name);
}

// Registers multiple component types
template<typename A, typename B, typename... Args>
void registerComponents()
{
    registerComponents<A>();
    registerComponents<B, Args...>();
}

}

#endif
