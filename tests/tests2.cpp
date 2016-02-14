// Copyright (C) 2015-2016 Eric Hebert (ayebear)
// This code is licensed under MIT, see LICENSE.txt for details.

#include "tests.h"
#include <iostream>

void someFunc()
{
    esTests::Position pos {50, 60};
    std::cout << pos.x << ", " << pos.y << "\n";
}
