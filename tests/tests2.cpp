// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#include "tests.h"
#include <iostream>

void someFunc()
{
    esTests::Position pos {50, 60};
    std::cout << pos.x << ", " << pos.y << "\n";
}
