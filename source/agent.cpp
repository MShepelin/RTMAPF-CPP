#include "agent.h"
#include <cassert>

SegmentSpace AgentOperations::MakeSpaceFromAgentShape(const SegmentSpace& base, const Agent& agent)
{
  assert(agent.shape.empty());
  // TODO fix for difficult shapes

  return base;
}
