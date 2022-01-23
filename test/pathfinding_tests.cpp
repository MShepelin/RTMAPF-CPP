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

      if (space->GetAccess(destination) == ACCESSABLE)
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

TEST(PathfindingTests, SimpleMap)
{
  std::shared_ptr<RawSpace> space(new RawSpace(3, 3));
  space->SetAccess({ 0, 0 }, ACCESSABLE);
  space->SetAccess({ 0, 1 }, ACCESSABLE);
  space->SetAccess({ 0, 2 }, ACCESSABLE);

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

  Pathfinder<Point, RawSpace> simplePathfinding(movesComponent, origin, space, h);

  simplePathfinding.FindCost(destination);
  ASSERT_TRUE(simplePathfinding.IsCostFound(destination));

  Time cost = simplePathfinding.GetCost(destination);
  ASSERT_EQ(cost, 2);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
