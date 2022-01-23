#pragma once

#include "search_types.h"
#include "space.h"

using AgentID = uint32_t;

struct Agent
{
  // TODO struct -> class, public and private methods

  AgentID id;

  Shape shape;
  // move component
};

namespace AgentOperations
{
  SegmentSpace MakeSpaceFromAgentShape(const SegmentSpace& base, const Agent& agent);
}
