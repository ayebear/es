// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_COMPONENTARRAY_H
#define ES_COMPONENTARRAY_H

#include "es/component.h"
#include "es/internal/packedarray.h"
#include "es/internal/id.h"
#include <memory>

namespace es
{

// An interface for using a PackedArray of any component type
class BaseComponentArray
{
    public:
        BaseComponentArray() {}
        virtual ~BaseComponentArray() {}

        virtual std::unique_ptr<BaseComponentArray> clone() const = 0;
        virtual ID copyFrom(const BaseComponentArray& baseSrcArray, ID id) = 0;

        virtual ID create() = 0;
        virtual Component& operator[] (ID id) = 0;
        virtual const Component& operator[] (ID id) const = 0;
        virtual Component* get(ID id) = 0;
        virtual const Component* get(ID id) const = 0;
        virtual bool isValid(ID id) const = 0;
        virtual void erase(ID id) = 0;
        virtual void clear() = 0;
        virtual size_t size() const = 0;
};

// A wrapper around a PackedArray designed for storing components
template <class T>
class ComponentArray: public BaseComponentArray
{
    public:
        ComponentArray() {}
        ~ComponentArray() {}

        virtual std::unique_ptr<BaseComponentArray> clone() const
        {
            return std::make_unique<ComponentArray<T>>(static_cast<const ComponentArray<T>&>(*this));
        }

        ID copyFrom(const BaseComponentArray& baseSrcArray, ID id)
        {
            return create(static_cast<const ComponentArray<T>&>(baseSrcArray)[id]);
        }

        ID create()
        {
            return array.create();
        }

        template <typename... Args>
        ID create(Args&&... args)
        {
            return array.create(std::forward<Args>(args)...);
        }

        T& operator[] (ID id)
        {
            return array[id];
        }

        const T& operator[] (ID id) const
        {
            return array[id];
        }

        T* get(ID id)
        {
            return array.get(id);
        }

        const T* get(ID id) const
        {
            return array.get(id);
        }

        bool isValid(ID id) const
        {
            return array.isValid(id);
        }

        void erase(ID id)
        {
            array.erase(id);
        }

        void clear()
        {
            array.clear();
        }

        size_t size() const
        {
            return array.size();
        }

        auto begin()
        {
            return array.begin();
        }

        auto end()
        {
            return array.end();
        }

        auto cbegin() const
        {
            return array.cbegin();
        }

        auto cend() const
        {
            return array.cend();
        }

    private:
        PackedArray<T> array;
};

}

#endif
