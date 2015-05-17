// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "tests.h"
#include <iostream>
#include <chrono>
#include <deque>
#include <cassert>
#include "es/packedarray.h"

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

    // Basic tests
    es::PackedArray<Test> elements;
    auto id = elements.create("Hello", 5);
    auto id2 = elements.create(Test{"Testing", -50});
    Test tmp{"Goodbye", 999};
    auto id3 = elements.create(tmp);
    std::cout << id << ", " << id2 << ", " << id3 << "\n";

    // Erase tests
    std::cout << "Size: " << elements.size() << "\n";
    for (auto& elem: elements)
        std::cout << elem.name << " ";
    std::cout << "\n";

    elements.erase(id);
    elements.erase(id);
    elements.erase(id3);
    elements.erase(id);

    std::cout << "Size: " << elements.size() << "\n";
    for (auto& elem: elements)
        std::cout << elem.name << " ";
    std::cout << "\n";

    // Handle tests
    auto id4 = elements.create("TEST2", 3141);
    std::cout << id4 << "\n";

    std::cout << "Size: " << elements.size() << "\n";
    for (auto& elem: elements)
        std::cout << elem.name << " ";
    std::cout << "\n";

    auto handle1 = elements.get(id4);
    std::cout << handle1.access().num << " x " << (*handle1).name << "\n";

    for (int i = 0; i < 1000000; ++i)
        elements.create("FILL", i);

    std::cout << handle1.access().num << " x " << handle1->name << "\n";

    elements.clear();

    // Performance tests

    es::PackedArray<size_t> array(20000000ULL);
    auto start = std::chrono::system_clock::now();
    std::cout << "Running benchmark 1... (creating)\n";
    for (size_t i = 0; i < 10000000ULL; ++i)
        array.create(i);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";

    std::vector<size_t> keys;
    keys.reserve(10000000ULL);

    start = std::chrono::system_clock::now();
    std::cout << "Running benchmark 1a... (accessing)\n";
    for (size_t i = 0; i < 10000000ULL; ++i)
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
    for (size_t i = 0; i < 10000000ULL; ++i)
        array.create(i);
    std::cout << "Done in " << getElapsedTime(start) << " seconds.\n";
    array.clear();
}

}
