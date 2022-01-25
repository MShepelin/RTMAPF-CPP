#include "pathfinder.h"
#include "space.h"
#include "hog2-utils/ScenarioLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

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

int main()
{
  std::ofstream animation(TEST_DATA_PATH "/animation.txt");
  if (!animation.is_open()) return 1;

  // Read map
  SpaceReader reader;
  std::ifstream spaceFile(TEST_DATA_PATH "/random-32-32-20.map");
  if (!spaceFile.is_open()) return 1;

  std::optional<RawSpace> rawSpace = reader.FromHogFormat(spaceFile);
  if (!rawSpace.has_value()) return 1;
  std::cout << "Map is ready\n";

  animation << rawSpace.value().GetWidth() << "\n";
  for (int i = 0; i < rawSpace.value().GetHeight(); ++i)
  {
    for (int j = 0; j < rawSpace.value().GetWidth(); ++j)
    {
      if (!rawSpace.value().GetAccess({ i, j }) == ACCESSABLE)
      {
        animation << "@";
      }
      else
      {
        animation << ".";
      }
    }

    animation << "\n";
  }

  Time depth = 300;
  std::shared_ptr<SpaceTime> space(new SpaceTime(depth, rawSpace.value()));
  
  // Read scenario
  ScenarioLoader loader(TEST_DATA_PATH "/random-32-32-20-even-1.scen");

  int agentsNum = 40;
  if (agentsNum > loader.GetNumExperiments())
  {
    std::cout << "too many agents, abort\n";
    return 1;
  }

  for (int i = 0; i < agentsNum; ++i)
  {
    Experiment agent = loader.GetNthExperiment(i);
    space->SetAccess({ {agent.GetStartX(), agent.GetStartY()}, {0, depth} }, INACCESSABLE);
  }

  for (int i = 0; i < agentsNum; ++i)
  {
    std::cout << "Planning agent " << i << "... ";

    // Prepare agent
    Experiment agent = loader.GetNthExperiment(i);
    Point start = { agent.GetStartX(), agent.GetStartY() };
    Point goal = { agent.GetGoalX(), agent.GetGoalY() };

    const SegmentHolder& segHolder = space->GetSegments(goal);
    const auto iter = --segHolder.end(); // todo assert if any exists
    Area origin = { start, {0, depth} };
    Area destination = { goal, *iter };

    space->SetAccess(origin, ACCESSABLE);

    // Prepare pathfinding
    ArrayType<Move<Point>> moves =
    {
      Move<Point>{ 1, {0, 1}},
      Move<Point>{ 1, {0, -1}},
      Move<Point>{ 1, {1, 0}},
      Move<Point>{ 1, {-1, 0}},
    };

    std::shared_ptr<MovesTestSegment> movesComponent(new MovesTestSegment(moves, space.get()));

    std::shared_ptr<EuclideanHeuristic> hpoint(new EuclideanHeuristic(destination.point));
    std::shared_ptr<Heuristic<Area>> h(new SpaceAdapter(hpoint));

    Pathfinder<Area> simplePathfinding(movesComponent, origin, h);

    // Execute pathfinding
    simplePathfinding.FindCost(destination);
    if (!simplePathfinding.IsCostFound(destination))
    {
      std::cout << "failed to find agent with id = " << i << "\n";
      break;
    }

    ArrayType<Node<Area>> path;
    simplePathfinding.CollectPath(destination, path);

    animation << "Agent " << i;
    for (const Node<Area>& node : path)
    {
      animation << " " << node.cell.point.x << " " << node.cell.point.y << " " << std::setprecision(4) << node.minTime;
    }

    animation << "\n";

    ArrayType<Area> inaccessableParts;
    FromPathToFilledAreas(path, inaccessableParts);
    space->MakeAreasInaccessable(inaccessableParts);

    std::cout << "success!\n";
  }
}
