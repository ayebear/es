// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_TESTS_H
#define ES_TESTS_H

#include "es/component.h"
#include "es/componentsetup.h"
#include "es/serialize.h"

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
void prototypeTests();
void eventTests();
void systemTests();

struct Position: public es::Component
{
    float x {0.0f};
    float y {0.0f};

    Position() {}
    Position(float x, float y): x{x}, y{y} {}

    void fromString(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string toString() const
    {
        return es::pack(x, y);
    }
};

struct Velocity: public es::Component
{
    float x {0.0f};
    float y {0.0f};

    Velocity() {}
    Velocity(float x, float y): x{x}, y{y} {}

    void fromString(const std::string& str)
    {
        es::unpack(str, x, y);
    }

    std::string toString() const
    {
        return es::pack(x, y);
    }
};

esRegister(Position);
esRegister(Velocity);

}

#endif
