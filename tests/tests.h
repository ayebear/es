// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_TESTS_H
#define ES_TESTS_H

#include <es/component.h>
#include <es/serialize.h>
#include <es/system.h>
#include <iostream>
#include <assert.h>

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

class System1: public es::System
{
    public:
        System1()
        {
            std::cout << "System1::System1()\n";
        }

        void initialize()
        {
            std::cout << "System1::initialize()\n";
        }

        void update(float dt)
        {
            std::cout << "System1::update(" << dt << ")\n";
        }

        void test() const
        {
            std::cout << "System1::test()\n";
        }
};

class System2: public es::System
{
    public:
        System2()
        {
            std::cout << "System2::System2()\n";
        }

        void initialize()
        {
            std::cout << "System2::initialize()\n";
        }

        void update(float dt)
        {
            std::cout << "System2::update(" << dt << ")\n";
        }
};

class System3: public es::System
{
    public:
        System3()
        {
            std::cout << "System3::System3()\n";
        }

        void initialize()
        {
            std::cout << "System3::initialize()\n";
        }

        void update(float dt)
        {
            std::cout << "System3::update(" << dt << ")\n";
        }
};

class System4: public es::System
{
    public:
        System4(const std::string& str):
            str(str)
        {
            std::cout << "System4::System4('" << str << "')\n";
        }

        void initialize()
        {
            std::cout << "System4::initialize()\n";
        }

        void update(float dt)
        {
            std::cout << "System4::update(" << dt << ")\n";
        }

    private:
        std::string str;
};

class System5: public es::System
{
    public:
        System5()
        {
            std::cout << "System5::System5()\n";
        }

        void initialize()
        {
            std::cout << "System5::initialize()\n";
        }

        void update(float dt)
        {
            std::cout << "System5::update()\n";
            assert(world != nullptr, "es::System::world is uninitialized");
        }
};

}

#endif
