#pragma once

#include "search_types.h"
#include <inttypes.h>
#include <vector>

using AgentID = uint32_t

struct Move
{
  Point deltaPoint;
};

struct MovementProperties
{
  Speed speed;
};

class Agent
{
  AgentID id;

  std::vector<Point> shape;
  MovementProperties movement;
  std::vector<Move> moves;
};
