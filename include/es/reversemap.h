// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_REVERSEMAP_H
#define ES_REVERSEMAP_H

#include <unordered_map>
#include <limits>

namespace es
{

/*
Similar to unordered_map, but supports getting keys by value.
*/
template <class Key, class Value>
class ReverseMap
{
    public:
        using ValueMap = std::unordered_map<Key, Value>;
        using KeyMap = std::unordered_map<Value, Key>;

        void insert(Key key, Value value)
        {
            values[key] = value;
            keys[value] = key;
        }

        Key getKey(Value value) const
        {
            auto found = keys.find(value);
            if (found != keys.end())
                return found->second;
            return invalidKey;
        }

        Value getValue(Key key) const
        {
            auto found = values.find(key);
            if (found != values.end())
                return found->second;
            return invalidValue;
        }

        KeyMap& getKeyMap()
        {
            return keys;
        }

        ValueMap& getValueMap()
        {
            return values;
        }

        void eraseByKey(Key key)
        {
            auto value = getValue(key);
            if (value != invalidValue)
            {
                keys.erase(value);
                values.erase(key);
            }
        }

        void eraseByValue(Value value)
        {
            auto key = getKey(value);
            if (key != invalidKey)
            {
                keys.erase(value);
                values.erase(key);
            }
        }

        void clear()
        {
            keys.clear();
            values.clear();
        }

        size_t size() const
        {
            return keys.size();
        }

        static const Key invalidKey = std::numeric_limits<Key>::max();
        static const Value invalidValue = std::numeric_limits<Value>::max();

    private:
        ValueMap values;
        KeyMap keys;
};

}

#endif
