// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_TESTS_H
#define ES_TESTS_H

#include <es/component.h>
#include <es/serialize.h>

namespace esTests
{

void runTests();
void packedArrayTests();
void packedArrayBenchmarks();
void serializationTests();
void componentTests();
void componentPoolTests();
void entityTests();
void worldTests();
void worldBenchmarks();
void prototypeTests();
void eventTests();
void systemTests();

struct Position: public es::Component
{
    static constexpr auto name = "Position";

    float x, y;

    Position(float x = 0, float y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};

struct Velocity: public es::Component
{
    static constexpr auto name = "Velocity";

    float x, y;

    Velocity(float x = 0, float y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};

struct Size: public es::Component
{
    static constexpr auto name = "Size";

    float x, y;

    Size(float x = 0, float y = 0): x{x}, y{y} {}

    void load(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string save() const
    {
        return es::pack(x, y);
    }
};

struct Sprite: public es::Component
{
    static constexpr auto name = "Sprite";

    std::string filename;

    Sprite(const std::string& filename = ""): filename(filename) {}

    void load(const std::string& str)
    {
        filename = str;
    }

    std::string save() const
    {
        return filename;
    }
};

}

#endif
