// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <string>
#include <sstream>

namespace es
{

/*
Internal functions for packing/unpacking using streams.
*/
void packStream(std::ostringstream& stream) {}

template <typename T, typename... Args>
void packStream(std::ostringstream& stream, const T& val, Args&&... args)
{
    stream << val;
    if (sizeof...(Args))
        stream << ' ';
    packStream(stream, args...);
}

unsigned unpackStream(std::istringstream& stream)
{
    return 0;
}

template <typename T, typename... Args>
unsigned unpackStream(std::istringstream& stream, T& val, Args&&... args)
{
    return !(stream >> val).fail() + unpackStream(stream, args...);
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
