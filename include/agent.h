#pragma once

#include "search_types.h"
#include "space.h"
#include <inttypes.h>
#include <vector>

using AgentID = uint32_t;

struct Move
{
  Speed speed = 0;
  Point deltaPoint;
};

struct Agent
{
  // TODO struct -> class, public and private methods

  AgentID id;

  std::vector<Point> shape;
  std::vector<Move> moves;
};

namespace AgentOperations
{
  static SegmentSpace MakeSpaceFromAgentShape(const SegmentSpace& base, const Agent& agent);
}
