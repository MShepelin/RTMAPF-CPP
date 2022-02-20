#pragma once

#include "search_types.h"
#include "space.h"
#include "shapes.h"

using AgentID = uint32_t;

struct Agent
{
  // TODO struct -> class, public and private methods

  AgentID id;
  Shape shape;
};
