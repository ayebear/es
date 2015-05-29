// Copyright (C) 2015 Eric Hebert (ayebear)
// This code is licensed under LGPLv3, see LICENSE.txt for details.

#ifndef ES_ID_H
#define ES_ID_H

#include <cstdint>

namespace es
{

using ID = uint64_t;
static const ID invalidId = std::numeric_limits<ID>::max();

}

#endif
