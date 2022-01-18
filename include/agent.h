#pragma once

#include "search_types.h"
#include "space.h"

using AgentID = uint32_t;

struct Agent
{
  // TODO struct -> class, public and private methods

  AgentID id;

  Shape shape;
  ArrayType<Move> moves;
};

namespace AgentOperations
{
  SegmentSpace MakeSpaceFromAgentShape(const SegmentSpace& base, const Agent& agent);
}
