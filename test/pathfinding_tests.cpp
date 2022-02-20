#include "pathfinder.h"
#include <gtest/gtest.h>

class MovesTest : public MoveComponent<Point>
{
private:
  RawSpace* space;
  ArrayType<Move<Point>> moves;

public:
  virtual ArrayType<Move<Point>> FindValidMoves(const Node<Point>& node) override
  {
    ArrayType<Move<Point>> result;
    Point origin = node.cell;

    for (Move<Point> move : moves)
    {
      Point destination = origin + move.destination;

      if (!space->Contains(destination)) continue;

      if (space->GetAccess(destination) == Access::Accessable)
      {
        result.push_back({ move.cost, destination});
      }
    }

    return result;
  }

  MovesTest(ArrayType<Move<Point>>& inmoves, RawSpace* inspace)
    : space(inspace)
    , moves(inmoves)
  {}
};

class MovesTestSegment : public MoveComponent<Area>
{
private:
  SegmentSpace* space;
  ArrayType<Move<Point>> moves;
  
public:
  virtual ArrayType<Move<Area>> FindValidMoves(const Node<Area>& node) override
  {
    ArrayType<Move<Area>> result;
    Area origin = node.cell;

    EXPECT_TRUE(space->ContainsSegmentsIn(origin.point));
    EXPECT_TRUE(space->Contains(origin));

    Segment moveAvailable{ node.minTime, node.cell.interval.end };

    for (Move<Point> move : moves)
    {
      Point destinationPoint = origin.point + move.destination;

      if (!space->ContainsSegmentsIn(destinationPoint)) continue;
      const SegmentHolder& segHolder = space->GetSegments(destinationPoint);

      for (Segment segment : segHolder)
      {
        Segment both = moveAvailable & segment;
      
        if (both.IsValid() && both.GetLength() >= move.cost)
        {
          Time overallCost = both.start + move.cost - node.minTime;
          EXPECT_TRUE(overallCost >= move.cost);
          result.push_back({ overallCost, Area{destinationPoint, segment} });
        }
      }
    }

    return result;
  }

  MovesTestSegment(ArrayType<Move<Point>>& inmoves, SegmentSpace* inspace)
    : space(inspace)
    , moves(inmoves)
  {}
};

TEST(PathfindingTests, SimpleMap)
{
  std::shared_ptr<RawSpace> space(new RawSpace(3, 3));
  space->SetAccess({ 0, 0 }, Access::Accessable);
  space->SetAccess({ 0, 1 }, Access::Accessable);
  space->SetAccess({ 0, 2 }, Access::Accessable);

  Point origin = { 0, 0 };
  Point destination = { 0, 2 };

  ArrayType<Move<Point>> moves =
  {
    Move<Point>{ 1, {0, 1}},
    Move<Point>{ 1, {0, -1}},
    Move<Point>{ 1, {1, 0}},
    Move<Point>{ 1, {-1, 0}},
  };

  std::shared_ptr<EuclideanHeuristic> h(new EuclideanHeuristic(destination));
  std::shared_ptr<MovesTest> movesComponent(new MovesTest(moves, space.get()));

  Pathfinder<Point> simplePathfinding(movesComponent, origin, h);

  simplePathfinding.FindCost(destination);
  ASSERT_TRUE(simplePathfinding.IsCostFound(destination));

  Time cost = simplePathfinding.GetCost(destination);
  ASSERT_EQ(cost, 2);
}

TEST(PathfindingTests, SegmentMap)
{
  std::shared_ptr<RawSpace> space(new RawSpace(3, 3));
  space->SetAccess({ 0, 0 }, Access::Accessable);
  space->SetAccess({ 0, 1 }, Access::Accessable);
  space->SetAccess({ 0, 2 }, Access::Accessable);

  Time depth = 4;
  std::shared_ptr<SegmentSpace> segmentSpace(new SegmentSpace(depth, *space));

  Area origin = { { 0, 0 }, {0, depth} };
  Area destination = { { 0, 2 }, {0, depth} };

  ArrayType<Move<Point>> moves =
  {
    Move<Point>{ 1, {0, 1}},
    Move<Point>{ 1, {0, -1}},
    Move<Point>{ 1, {1, 0}},
    Move<Point>{ 1, {-1, 0}},
  };

  std::shared_ptr<MovesTestSegment> movesComponent(new MovesTestSegment(moves, segmentSpace.get()));

  std::shared_ptr<EuclideanHeuristic> hpoint(new EuclideanHeuristic(destination.point));
  std::shared_ptr<Heuristic<Area>> h(new SpaceAdapter<Point, Area>(hpoint));

  Pathfinder<Area> simplePathfinding(movesComponent, origin, h);

  simplePathfinding.FindCost(destination);
  ASSERT_TRUE(simplePathfinding.IsCostFound(destination));

  Time cost = simplePathfinding.GetCost(destination);
  ASSERT_EQ(cost, 2);

  ArrayType<Node<Area>> path;
  simplePathfinding.CollectPath(destination, path);

  ASSERT_EQ(path.size(), 3);
  ASSERT_EQ(path[0].minTime, 0);
  ASSERT_EQ(path[0].cell, origin);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
