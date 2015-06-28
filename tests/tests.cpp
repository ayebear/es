// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "tests.h"
#include <iostream>
#include <chrono>
#include <deque>
#include <cassert>
#include <algorithm>
#include "es/internal/packedarray.h"
#include "es/componentpool.h"
#include "es/serialize.h"
#include "es/world.h"
#include "es/entityprototypeloader.h"

int main()
{
    esTests::runTests();
    return 0;
}

namespace esTests
{

double getElapsedTime(const auto& start)
{
    return std::chrono::duration<double>(std::chrono::system_clock::now() - start).count();
}

void runTests()
{
    es::registerComponents<Position, Velocity, Size, Sprite>();
    std::cout << "Running all tests...\n";
    packedArrayTests();
    packedArrayBenchmarks();
    serializationTests();
    componentTests();
    componentPoolTests();
    entityTests();
    worldTests();
    worldBenchmarks();
    prototypeTests();
    eventTests();
    systemTests();
    std::cout << "All tests passed!\n";
}

void packedArrayTests()
{
    struct Test
    {
        Test(const std::string& name = "", int num = 0): name(name), num(num) {}
        std::string name;
        int num;
    };

    // Create tests
    es::PackedArray<Test> elements;
    auto id = elements.create("Hello", 5);
    auto id2 = elements.create(Test{"Testing", -50});
    Test tmp{"Goodbye", 999};
    auto id3 = elements.create(tmp);

    assert(id == 0x0000000100000000ULL);
    assert(id2 == 0x0000000100000001ULL);
    assert(id3 == 0x0000000100000002ULL);
    assert(elements[id].num == 5 && elements[id2].num == -50 && elements[id3].num == tmp.num);

    std::vector<es::ID> idList;
    for (int i = 0; i < 3; ++i)
        idList.push_back(elements.create(tmp));
    for (auto elemId: idList)
        elements.erase(elemId);

    // Erase tests
    assert(elements.size() == 3);

    elements.erase(id);
    elements.erase(id);
    elements.erase(id3);
    elements.erase(id);
    auto tmpId = elements.create();
    elements.erase(id);
    elements.erase(tmpId);
    elements.erase(id);

    assert(elements.size() == 1);
    assert(elements.begin()->num == elements[id2].num && elements[id2].num == -50);

    elements.erase(id2);
    assert(elements.size() == 0);

    // Handle tests
    auto id4 = elements.create("TEST2", 3141);

    es::PID pid{id4};
    assert(pid.version == 2);
    assert(pid.index == 1);
    assert(id4 == 0x0000000200000001ULL);
    assert(elements.size() == 1);

    auto handle1 = elements.getHandle(id4);
    assert(handle1.access().num == 3141);
    assert((*handle1).num == 3141);
    assert(handle1->num == 3141);
    assert(handle1.access().name == "TEST2");
    assert((*handle1).name == "TEST2");
    assert(handle1->name == "TEST2");
    assert(handle1.get()->name == "TEST2");

    // This should cause some reallocations
    for (int i = 0; i < 10000; ++i)
        elements.create("FILL", i);

    assert(handle1.access().num == 3141);
    assert((*handle1).num == 3141);
    assert(handle1->num == 3141);
    assert(handle1.access().name == "TEST2");
    assert((*handle1).name == "TEST2");
    assert(handle1->name == "TEST2");
    assert(handle1.get()->name == "TEST2");

    assert(handle1);
    assert(handle1.valid());
    assert(handle1.get());
    elements.clear();
    assert(!handle1);
    assert(!handle1.valid());
    assert(!handle1.get());

    // Test the case were the element on the end is swapped
    auto idA = elements.create();
    auto idB = elements.create();
    auto idC = elements.create();
    auto idD = elements.create("Good", 100);
    auto handleD = elements.getHandle(idD);
    assert(handleD->name == "Good");
    elements.erase(idB);
    assert(handleD->name == "Good");
    elements.erase(idC);
    assert(handleD->name == "Good");
    elements.erase(idA);
    assert(handleD->name == "Good");

    // Const handle tests
    elements.clear();
    auto constId = elements.create("Const", 999);
    const auto& constElems = elements;
    auto constHandle = constElems.getHandle(constId);
    assert(constHandle && constHandle->name == "Const" && constHandle.access().num == 999 && constHandle.get()->num == 999);

    // Invalid elements
    es::PackedArray<std::string> strs;
    auto invId = strs.create("test1");
    assert(strs[invId] == "test1");
    assert(*strs.get(invId) == "test1");
    strs.erase(invId);
    assert(!strs.get(invId));

    // Invalid elements with different delete order cases
    auto testId1 = strs.create("test1");
    auto testId2 = strs.create("test2");
    auto testId3 = strs.create("test3");
    assert(strs[testId1] == "test1" && strs[testId2] == "test2" && strs[testId3] == "test3");
    strs.erase(testId3);
    assert(strs[testId1] == "test1" && strs[testId2] == "test2" && !strs.get(testId3));
    strs.erase(testId1);
    assert(!strs.get(testId1) && strs[testId2] == "test2" && !strs.get(testId3));
    strs.erase(testId2);
    assert(!strs.get(testId1) && !strs.get(testId2) && !strs.get(testId3));

    std::cout << "PackedArray tests passed.\n";
}

void packedArrayBenchmarks()
{
    const size_t numElems = 1000000;

    es::PackedArray<size_t> array(numElems);
    auto start = std::chrono::system_clock::now();
    std::cout << "Running benchmark 1... (creating)\n";
    for (size_t i = 0; i < numElems; ++i)
        array.create(i);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    std::vector<size_t> keys;
    keys.reserve(numElems);

    start = std::chrono::system_clock::now();
    std::cout << "Running benchmark 1a... (accessing)\n";
    for (size_t i = 0; i < numElems; ++i)
        keys.push_back(array[i]);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    start = std::chrono::system_clock::now();
    std::cout << "Running benchmark 1b... (erasing)\n";
    for (auto& key: keys)
        array.erase(key);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    array.clear();

    start = std::chrono::system_clock::now();
    std::cout << "Running benchmark 2... (creating)\n";
    for (size_t i = 0; i < numElems; ++i)
        array.create(i);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";
    array.clear();
}

void serializationTests()
{
    // Packing data into a string
    std::string packText("TEST");
    auto str = es::pack("hey", "there", 25, 3.141, packText, 99999999999999ULL);
    assert(str == "hey there 25 3.141 TEST 99999999999999");

    // Unpacking data from a string
    float dt;
    std::string text;
    int num;
    auto count = es::unpack("0.0123 testing 567", dt, text, num);
    assert(count == 3);
    assert(dt > 0.01f && dt < 0.02f && text == "testing" && num == 567);

    // Unpack (data > args)
    int num2;
    auto count2 = es::unpack("3 2 1", num2);
    assert(count2 == 1);
    assert(num2 == 3);

    // Unpack (data < args)
    int num3 = 500;
    double dec3 = 1.2345;
    std::string str3 = "test";
    auto count3 = es::unpack("999", num3, dec3, str3);
    assert(count3 == 1);
    assert(num3 == 999 && dec3 == 0.0 && str3.empty());

    // Rare cases
    auto count4 = es::unpack("some data");
    assert(count4 == 0);
    auto count5 = es::unpack("");
    assert(count5 == 0);
    int num6a, num6b, num6c;
    auto count6 = es::unpack("", num6a, num6b, num6c);
    assert(count6 == 0);
    auto str7 = es::pack();
    assert(str7.empty());
    auto str8 = es::pack("");
    assert(str8.empty());
    auto str9 = es::pack(5.4321);
    assert(str9 == "5.4321");

    std::cout << "Serialization tests passed.\n";
}

void componentTests()
{
    Position posComp;
    posComp.load("22.1 97.3");
    assert(posComp.save() == "22.1 97.3");
    std::string posStr = posComp;
    assert(posStr == "22.1 97.3");
    es::Component& baseComp = posComp;
    posStr = baseComp;
    assert(posStr == "22.1 97.3");

    // From the name querying loop in the readme
    es::World world;
    auto ent = world.create().assign<Position>(12, 15).assign<Velocity>(1, 2);
    auto pos = ent.get("Position");
    auto vel = ent.get("Velocity");
    float x1, y1, x2, y2;
    es::unpack(*pos, x1, y1);
    es::unpack(*vel, x2, y2);
    x1 += x2;
    y1 += y2;
    *pos = es::pack(x1, y1);
    assert(pos->save() == "13 17");

    std::cout << "Component tests passed.\n";
}

void componentPoolTests()
{
    // Create component pool and a component
    es::ComponentPool comps;
    auto posId = comps.get<Position>().create(100, 200);

    // Type-based access -----------------------------------------------------

    // References
    auto& posRef = comps.get<Position>()[posId];
    assert(posRef.x == 100 && posRef.y == 200);

    // Handles
    // auto posHandle = comps.get<Position>().getHandle(posId);
    // assert(posHandle && posHandle->x == 100 && posHandle->y == 200);

    // String-based access ---------------------------------------------------

    // Lookup tests
    assert(comps["TEST"] == nullptr);
    assert(comps["Position"] != nullptr);

    // References
    auto& baseComp = (*comps["Position"])[posId];
    assert(baseComp.save() == "100 200");
    baseComp = "555 963";
    assert(baseComp.save() == "555 963");

    // Handles
    // auto baseHandle = comps["Position"]->getBaseHandle(posId);
    // assert(baseHandle && baseHandle->save() == "555 963");

    std::cout << "ComponentPool tests passed.\n";
}

void entityTests()
{
    es::World world;
    auto ent = world.create();

    // Assigning components
    ent.assign<Position>(55, 67).assign<Velocity>(97, 650);

    ent.assignFrom(Position(1, 2), Velocity(6, 2));

    ent << Position(100, 200) << Velocity(150, 300);

    ent.assign<Sprite>("test.png");

    auto pos = ent.getPtr<Position>();
    assert(pos && pos->x > 99.0f && pos->x < 101.0f);

    auto posHandle = ent.get<Position>();
    assert(posHandle && posHandle->x > 99.0f && posHandle->x < 101.0f);

    // Checking if components exist
    bool hasComps = ent.has<Position, Velocity>();
    assert(hasComps);
    hasComps = ent.has<std::string, std::string, Position, Velocity, int, std::string>();
    assert(!hasComps);
    hasComps = ent.has<std::string>();
    assert(!hasComps);

    assert(ent.has("Position"));
    assert(ent.has("Position", "Velocity"));
    assert(!ent.has("Position", "Velocity", "Unknown"));

    // Removing components
    assert(ent.total() == 3);
    ent.clear();
    assert(ent.total() == 0);

    ent << Position(100, 200) << Velocity(150, 300);
    assert(ent.total() == 2);
    ent.remove<Position>();
    assert(ent.total() == 1);
    ent.remove<Velocity>();
    assert(ent.total() == 0);

    ent << Position(100, 200) << Velocity(150, 300);
    assert(ent.total() == 2);
    ent.remove<Position, Velocity>();
    assert(ent.total() == 0);
    ent.remove<std::string, int, float>();
    assert(ent.total() == 0);

    ent << Position(100, 200) << Velocity(150, 300);
    assert(ent.total() == 2);
    ent.remove("Position", "Velocity");
    assert(ent.total() == 0);
    ent.remove("Position", "", "invalid");
    assert(ent.total() == 0);

    // Accessing components
    ent << Position(10, 50) << Velocity(20, 40);
    assert(ent.at<Position>()->x == 10);
    assert(ent.get<Position>()->y == 50);
    assert(ent.getPtr<Velocity>()->x == 20);
    assert(ent.access<Velocity>().y == 40);

    assert(ent.get("Position")->save() == "10 50");
    assert(ent.getPtr("Position")->save() == "10 50");
    assert(ent.at("Position")->save() == "10 50");
    assert(ent.access("Position").save() == "10 50");
    assert(ent["Position"].save() == "10 50");

    ent.clear();
    ent.clear();
    ent.clear();

    ent.at("Position")->load("80 85");
    assert(ent["Position"].save() == "80 85");
    ent["Velocity"].load("98 99");
    assert(ent["Velocity"].save() == "98 99");
    ent.clear();
    auto hndl = ent.at<Position>();
    ent.access<Velocity>().x = 7;
    ent.access<Velocity>().y = 8;
    hndl->x = 5;
    hndl->y = 6;
    assert(ent["Velocity"].save() == "7 8");
    std::string posString("Position");
    assert(ent[posString].save() == "5 6");
    ent.clear();

    ent << Position(900, 800);
    Position posComp;
    ent >> posComp;
    assert(posComp.x == 900 && posComp.y == 800);
    ent.clear();
    Position posComp2 {0, 0};
    ent >> posComp2;
    assert(posComp2.x == 0 && posComp2.y == 0);
    ent.clear();

    // Invalid accessing of components
    assert(ent.total() == 0);
    assert(!ent.getPtr<Velocity>());
    assert(!ent.get<Position>());
    assert(!ent.getPtr("Velocity"));
    assert(!ent.getPtr("Invalid"));
    assert(!ent.getPtr(""));
    assert(!ent.get("Velocity"));
    assert(!ent.get("Invalid"));
    assert(!ent.get(""));
    assert(world.validName("Position"));
    assert(!world.validName("testing"));

    // Getting all component names
    ent.clear();
    ent << Position(100, 100) << Sprite("testing.png");
    auto names = ent.getNames();
    assert(std::find(names.begin(), names.end(), "Sprite") != names.end());
    assert(std::find(names.begin(), names.end(), "Position") != names.end());
    assert(std::find(names.begin(), names.end(), "Velocity") == names.end());

    // Iterating though component names
    for (const auto& name: names)
        ent.remove(name);
    assert(ent.empty());

    std::cout << "Entity tests passed.\n";
}

void worldTests()
{
    // Create entities
    es::World world;
    auto ent = world.create();
    auto ent2 = world.create("namedEntity");
    auto ent3 = world["test"];
    assert(ent && ent2 && ent3);
    assert(ent.getName().empty() && ent2.getName() == "namedEntity" && ent3.getName() == "test");

    // Destroy entities directly
    ent.destroy();
    ent2.destroy();
    ent3.destroy();
    assert(!ent && !ent2 && !ent3);

    // Destroy entities from world
    auto entD1 = world.create();
    world.destroy(entD1.getId());
    assert(!entD1);
    auto entD2 = world.create("toDestroy");
    world.destroy("toDestroy");
    assert(!entD2);

    // Create/access
    world["test2"]["Position"].load("250 300");
    assert(world["test2"]["Position"].save() == "250 300");
    assert(world["test2"].at<Position>()->x == 250);
    world["test2"].destroy();
    world.clear();

    // Valid tests
    auto validEnt = world.create("validTest");
    auto validEntId = validEnt.getId();
    assert(validEnt.getName() == "validTest");
    assert(world.valid(validEntId));
    assert(world.valid("validTest"));
    world.destroy("validTest");
    world.destroy(validEnt.getId());
    validEnt.destroy();
    world.destroy(validEnt.getId());
    world.destroy("validTest");
    assert(validEnt.getName().empty());
    assert(!world.valid(validEntId));
    assert(!world.valid("validTest"));

    // Cloning entities (same world)
    auto orig = world["original"];
    orig << Position(90, 95) << Velocity(85, 80);
    auto clone1 = orig.clone();
    auto clone2 = orig.clone("cloned");
    assert(orig.getName() == "original" && clone1.getName().empty() && clone2.getName() == "cloned");
    assert(orig["Position"].save() == "90 95" && orig["Velocity"].save() == "85 80");
    assert(clone1["Position"].save() == "90 95" && clone1["Velocity"].save() == "85 80");
    assert(clone2["Position"].save() == "90 95" && clone2["Velocity"].save() == "85 80");

    // Cloning entities (between worlds)
    es::World world2;
    auto clone3 = orig.clone(world2);
    auto clone4 = orig.clone(world2, "clone4");
    assert(clone3.getName().empty() && clone4.getName() == "clone4");
    assert(clone3["Position"].save() == "90 95" && clone3["Velocity"].save() == "85 80");
    assert(clone4["Position"].save() == "90 95" && clone4["Velocity"].save() == "85 80");

    // Cloning invalid entities
    auto ent4 = world.create();
    ent4.destroy();
    assert(!ent4);
    auto ent5 = ent4.clone();
    assert(ent5);
    auto ent6 = ent4.clone(world2);
    assert(ent6);

    world["queryTest"] << Position(10, 10) << Velocity(20, 20) << Sprite("test.png");

    // Iterating through all entities
    for (auto ent: world.query())
    {
        auto pos = ent.get<Position>();
        auto vel = ent.get<Velocity>();
        auto sprite = ent.get<Sprite>();
        if (pos && vel && sprite)
        {
            pos->x = 1;
            pos->y = 2;
            vel->x = 3;
            vel->y = 4;
            sprite->filename = "sprite.png";
            assert(pos->save() == "1 2");
            assert(vel->save() == "3 4");
            assert(sprite->save() == "sprite.png");
        }
    }

    // Querying by type
    for (auto ent: world.query<Position, Velocity, Sprite>())
    {
        auto pos = ent.get<Position>();
        auto vel = ent.get<Velocity>();
        pos->x += vel->x;
        pos->y += vel->y;
        assert(pos->save() == "4 6");
        assert(vel->save() == "3 4");
    }

    // Querying by name
    for (auto ent: world.query("Position", "Velocity", "Sprite"))
    {
        auto pos = ent.get("Position");
        auto vel = ent.get("Velocity");
        float x1, y1, x2, y2;
        es::unpack(*pos, x1, y1);
        es::unpack(*vel, x2, y2);
        x1 += x2;
        y1 += y2;
        *pos = es::pack(x1, y1);
        assert(pos->save() == "7 10");
        assert(vel->save() == "3 4");
    }

    // Iterating directly through components
    world.clear();
    world["test1"] << Position(20, 30);
    world["test2"] << Position(20, 30);
    world["test3"] << Position(20, 30);
    size_t numComponents = 0;
    for (auto& pos: world.getComponents<Position>())
    {
        ++numComponents;
        pos.x += 5;
        pos.y += 10;
        assert(pos.x == 25 && pos.y == 40);
    }
    assert(numComponents == 3);
    for (const auto& pos: world.getComponents<Position>())
        assert(pos.x == 25 && pos.y == 40);

    // Handle pointer tests with the world
    auto test1Ent = world["test1"];
    auto ptr1 = test1Ent.get<Position>().get();
    assert(ptr1 && ptr1->x == 25);
    auto ptr2 = test1Ent.get<Velocity>().get();
    assert(!ptr2);
    auto ptr3 = test1Ent.get("Position").get();
    assert(ptr3 && ptr3->save() == "25 40");
    auto ptr4 = test1Ent.get("Invalid").get();
    assert(!ptr4);

    std::cout << "World tests passed.\n";
}

void worldBenchmarks()
{
    es::loadPrototypes("entities.cfg");
    es::World world;

    // Create some empty entities
    auto start = std::chrono::system_clock::now();
    std::cout << "Creating random entities...\n";
    for (size_t i = 0; i < 1000000; ++i)
    {
        auto ent = world.create();
        if (rand() % 3)
            ent.assign<Position>(rand() % 30, rand() % 50);
        else if (rand() % 3)
            ent.assign<Sprite>("Some string");
        else
        {
            ent.assign<Position>(rand() % 500, rand() % 400);
            ent.assign<Velocity>(rand() % 80, rand() % 60);
        }
    }
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    start = std::chrono::system_clock::now();
    std::cout << "Querying...\n";
    auto result = world.query<Position, Velocity>();
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    start = std::chrono::system_clock::now();
    std::cout << "Iterating/assigning...\n";
    for (auto ent: result)
        ent.assign<Position>(20, 25);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    start = std::chrono::system_clock::now();
    std::cout << "Iterating/assigning by name...\n";
    for (auto ent: result)
        ent["Position"] = "25 20";
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    std::cout << "World benchmarks done.\n";
}

void prototypeTests()
{
    // Deserialization tests
    es::World world;
    auto ent = world.create("cool");
    ent << "Position 123 789" << Velocity(333, 444);
    assert(ent["Position"].save() == "123 789");
    assert(ent["Velocity"].save() == "333 444");
    ent << "Position";
    assert(ent["Position"].save() == "0 0");
    ent.deserialize("Position", "678 321");
    assert(ent["Position"].save() == "678 321");

    // Serialization tests
    auto comps = ent.serialize();
    std::sort(comps.begin(), comps.end());
    assert(comps.size() == 2);
    assert(comps[0] == "Position 678 321");
    assert(comps[1] == "Velocity 333 444");
    ent << "Sprite";
    assert(ent.serialize().size() == 3);

    // Load prototypes
    bool status = es::loadPrototypes("entities.cfg");
    assert(status);

    // Create entities from prototypes
    auto box = world.copy("Box");
    assert(box.total() == 3);
    assert(box["Size"].save() == "64 64");
    assert(box.getName().empty());

    auto player = world.copy("Player", "player");
    assert(player.total() == 2);
    assert(player["Position"].save() == "50 10");
    assert(player.getName() == "player");
    assert(player.getId() == world["player"].getId());

    // Testing operator() overloads
    auto player2 = world("Player", "player2");
    assert(player2);
    assert(player2.total() == 2);
    assert(player2.getName() == "player2");

    auto player3 = world("Player", "");
    assert(player3);
    assert(player3.total() == 2);
    assert(player3.getName().empty());

    auto player4 = world("", "player4");
    assert(player4);
    assert(player4.total() == 0);
    assert(player4.getName() == "player4");

    auto player5 = world("", "");
    assert(player5);
    assert(player5.total() == 0);
    assert(player5.getName().empty());

    // Invalid prototype names
    auto ent2 = world.copy("");
    assert(ent2 && ent2.total() == 0);
    auto ent3 = world.copy("invalid");
    assert(ent3 && ent3.total() == 0);
}

void eventTests()
{

}

void systemTests()
{

}




}
