// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "tests.h"
#include <iostream>
#include <chrono>
#include <deque>
#include <cassert>
#include "es/packedarray.h"
#include "es/componentpool.h"
#include "es/serialize.h"

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
    std::cout << "Running all tests...\n";
    packedArrayTests();
    // packedArrayBenchmarks();
    serializationTests();
    componentTests();
    componentPoolTests();
    entityTests();
    worldTests();
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

    assert(id == 0 && id2 == 1 && id3 == 2);
    assert(elements[id].num == 5 && elements[id2].num == -50 && elements[id3].num == tmp.num);

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

    assert(id4 == 4);
    assert(elements.size() == 1);

    auto handle1 = elements.getHandle(id4);
    assert(handle1.access().num == 3141);
    assert((*handle1).num == 3141);
    assert(handle1->num == 3141);
    assert(handle1.access().name == "TEST2");
    assert((*handle1).name == "TEST2");
    assert(handle1->name == "TEST2");

    // This should cause some reallocations
    for (int i = 0; i < 10000; ++i)
        elements.create("FILL", i);

    assert(handle1.access().num == 3141);
    assert((*handle1).num == 3141);
    assert(handle1->num == 3141);
    assert(handle1.access().name == "TEST2");
    assert((*handle1).name == "TEST2");
    assert(handle1->name == "TEST2");

    elements.clear();

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
    assert(constHandle && constHandle->name == "Const" && constHandle.access().num == 999);
}

void packedArrayBenchmarks()
{
    uint64_t numElems = 100000ULL;

    es::PackedArray<size_t> array(numElems * 2);
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
    std::string text("TEST");
    auto str = es::pack("hey", "there", 25, 3.141, text, 99999999999999ULL);
    assert(str == "hey there 25 3.141 TEST 99999999999999");

    // Unpacking data from a string
    float dt;
    std::string text2;
    int num;
    auto count = es::unpack("0.0123 testing 567", dt, text2, num);
    assert(count == 3);
    assert(dt > 0.01f && dt < 0.02f && text2 == "testing" && num == 567);
}

void componentTests()
{
    Position pos;
    pos.fromString("22.1 97.3");
    assert(pos.toString() == "22.1 97.3");
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
    auto posHandle = comps.get<Position>().getHandle(posId);
    assert(posHandle && posHandle->x == 100 && posHandle->y == 200);

    // String-based access ---------------------------------------------------

    // Lookup tests
    assert(comps["TEST"] == nullptr);
    assert(comps["Position"] != nullptr);

    // References
    auto& baseComp = (*comps["Position"])[posId];
    assert(baseComp.toString() == "100 200");
    baseComp = "555 963";
    assert(baseComp.toString() == "555 963");

    // Handles
    auto baseHandle = comps["Position"]->getBaseHandle(posId);
    assert(baseHandle && baseHandle->toString() == "555 963");
}

void entityTests()
{

}

void worldTests()
{

}

void eventTests()
{

}

void systemTests()
{

}




}
