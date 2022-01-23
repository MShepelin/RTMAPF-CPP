/*
#include "pathfinder.h"
#include <gtest/gtest.h>

TEST(PathfindingTests, SimpleMap)
{
  std::shared_ptr<RawSpace> space(new RawSpace(3, 3));
  space->SetAccess({ 0, 0 }, ACCESSABLE);
  space->SetAccess({ 1, 1 }, ACCESSABLE);
  space->SetAccess({ 2, 2 }, ACCESSABLE);

  ArrayType<Area> moves = {
    { 1, {1, 0} },
    { 1, {0, 1} },
    { 1, {-1, 0} },
    { 1, {0, -1} }
  };

  Pathfinder<Access, Point> simplePathfinding(moves, { 0, 0 }, space);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
*/