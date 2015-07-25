// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_SERIALIZE_H
#define ES_SERIALIZE_H

#include <string>
#include <sstream>

namespace es
{

/*
Internal functions for packing/unpacking using streams.
*/
template <typename T>
bool notEmpty(const T& val)
{
    return true;
}

inline bool notEmpty(const std::string& val)
{
    return !val.empty();
}

inline bool notEmpty(const char* val)
{
    return (val && *val);
}

template <typename T>
void addToStream(std::ostringstream& stream, const T& val)
{
    if (notEmpty(val))
    {
        if (!stream.str().empty())
            stream << ' ';
        stream << val;
    }
}

inline void packStream(std::ostringstream& stream) {}

template <typename T, typename... Args>
void packStream(std::ostringstream& stream, const T& val, Args&&... args)
{
    addToStream(stream, val);
    packStream(stream, args...);
}

inline unsigned unpackStream(std::istringstream& stream)
{
    return 0;
}

template <typename T, typename... Args>
unsigned unpackStream(std::istringstream& stream, T& val, Args&&... args)
{
    unsigned parsed = !(stream >> val).fail();
    if (!parsed)
        val = T{};
    return parsed + unpackStream(stream, args...);
}

/*
Joins variables together into a single string.
Usage:
    auto str = es::pack(50, "test", 3.141);
    str: "50 test 3.141"
*/
template <typename... Args>
std::string pack(Args&&... args)
{
    std::ostringstream stream;
    packStream(stream, args...);
    return stream.str();
}

/*
Extracts values from a string and stores them into variables.
Returns the number of values extracted.
Notes:
    When there is more data than arguments, it is ignored.
    When there are more arguments than data, the remaining arguments
        are default-initialized.
Usage:
    float dt;
    std::string text;
    int num;
    es::unpack("0.0123 testing 500", dt, text, num);
*/
template <typename... Args>
unsigned unpack(const std::string& data, Args&&... args)
{
    std::istringstream stream(data);
    return unpackStream(stream, args...);
}

}

#endif
