#pragma once

#include "search_types.h"
#include "space.h"
#include "shapes.h"
#include <cmath>
#include "moves.h"

class MovesTestSegment : public MoveComponent<Area>, public MoveComponent<Point>
{
protected:
  Time depth;
  std::shared_ptr<ShapeSpace> space;
  ArrayType<Move<Point>> moves;

public:
  virtual ArrayType<Move<Area>> FindValidMoves(const Node<Area>& node) override
  {
    ArrayType<Move<Area>> result;
    Area origin = node.cell;

    Segment moveAvailable{ node.minTime, node.cell.interval.end };
    if (node.cell.interval.end >= depth)
    {
      result.push_back({ 0, Area{origin.point, {depth, depth}}, moveAvailable.GetLength() });
    }

    for (Move<Point> move : moves)
    {
      Point destinationPoint = origin.point + move.destination;

      space->UpdateShape(destinationPoint);
      if (!space->ContainsSegmentsIn(destinationPoint)) continue;
      const SegmentHolder& segHolder = space->GetSegments(destinationPoint);

      for (Segment segment : segHolder)
      {
        // TODO mechanism to check collision with other cells (example: move from (0, 0) to (5, 5)
        Segment both = moveAvailable & segment;

        if (both.IsValid() && both.GetLength() >= move.moveCost)
        {
          result.push_back({ move.moveCost, Area{destinationPoint, segment}, both.start - node.minTime });
        }
      }
    }

    return result;
  }

  virtual ArrayType<Move<Point>> FindValidMoves(const Node<Point>& node) override
  {
    ArrayType<Move<Point>> result;
    Point origin = node.cell;

    for (Move<Point> move : moves)
    {
      Point destinationPoint = origin + move.destination;

      space->UpdateShape(destinationPoint);
      if (!space->ContainsSegmentsIn(destinationPoint)) continue;

      const SegmentHolder& segHolder = space->GetSegments(destinationPoint);
      if (segHolder.end() == segHolder.begin()) continue;

      // TODO mechanism to check collision with other cells (example: move from (0, 0) to (5, 5)
      result.push_back({ move.moveCost, destinationPoint });
    }

    return result;
  }

  MovesTestSegment(ArrayType<Move<Point>>& inmoves, std::shared_ptr<ShapeSpace> inspace, Time inDepth)
    : space(inspace)
    , moves(inmoves)
    , depth(inDepth)
  {}
};

using AgentID = uint32_t;

struct Agent
{
  // TODO struct -> class, public and private methods

  AgentID id;
  Shape shape;

  Point goal;

  float currentX;
  float currentY;

  ArrayType<Move<Point>> moves =
  {
    Move<Point>{ 1, {0, 1}},
    Move<Point>{ 1, {0, -1}},
    Move<Point>{ 1, {1, 0}},
    Move<Point>{ 1, {-1, 0}},
    Move<Point>{ std::sqrt(2.f), {1, 1}},
    Move<Point>{ std::sqrt(2.f), {-1, -1}},
    Move<Point>{ std::sqrt(2.f), {1, -1}},
    Move<Point>{ std::sqrt(2.f), {-1, 1}},
  };

  Point GetCurrentPoint()
  {
    return { (int) std::floor(currentX), (int) std::floor(currentY) };
  }
};
