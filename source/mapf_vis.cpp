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

class Mission
{
  ScenarioLoader loader;
  std::shared_ptr<SpaceTime> space;
  std::ofstream animation;
  Time depth = 0;
  int agentsNum = 0;

public:
  Mission(const char* scenariosFileName)
    : loader(scenariosFileName)
  {
    assert(loader.GetNumExperiments() > 0);
  }

  int InitAnimation(const char* animationFileName)
  {
    std::cout << animationFileName << "\n";
    animation.open(animationFileName, std::ios_base::out);
    if (!animation.is_open()) return 1;
    return 0;
  }

  int ReadSpace(Time inDepth, const char* spaceFileName)
  {
    if (!animation.is_open()) return 1;

    SpaceReader reader;
    std::ifstream spaceFile(spaceFileName);
    if (!spaceFile.is_open()) return 1;

    std::optional<RawSpace> rawSpace = reader.FromHogFormat(spaceFile);
    if (!rawSpace.has_value()) return 1;

    depth = inDepth;
    space = std::make_shared<SpaceTime>(inDepth, rawSpace.value());

    animation << rawSpace.value().GetWidth() << "\n";
    for (int i = 0; i < (int)rawSpace.value().GetHeight(); ++i)
    {
      for (int j = 0; j < (int)rawSpace.value().GetWidth(); ++j)
      {
        if (rawSpace.value().GetAccess({ i, j }) != Access::Accessable)
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

    return 0;
  }

  int InitAgents(int numAgents)
  {
    agentsNum = numAgents;
    if (agentsNum > loader.GetNumExperiments())
    {
      return 1;
    }

    for (int i = 0; i < agentsNum; ++i)
    {
      Experiment agent = loader.GetNthExperiment(i);
      space->SetAccess({ {agent.GetStartX(), agent.GetStartY()}, {0, depth} }, Access::Inaccessable);
    }

    return 0;
  }

  int SolveCycle()
  {
    for (int i = 0; i < agentsNum; ++i)
    {
      // TODO add test when agent stands on one place

      std::cout << "Planning agent " << i << "... ";

      // Prepare agent
      Experiment agent = loader.GetNthExperiment(i);
      Point start = { agent.GetStartX(), agent.GetStartY() };
      Point goal = { agent.GetGoalX(), agent.GetGoalY() };

      const SegmentHolder& segHolder = space->GetSegments(goal);
      const auto iter = --segHolder.end(); // todo assert if any exists
      Area origin = { start, {0, depth} };
      Area destination = { goal, *iter };

      space->SetAccess(origin, Access::Accessable);

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
      std::shared_ptr<Heuristic<Area>> h(new SpaceAdapter<Point, Area>(hpoint));

      Pathfinder<Area> simplePathfinding(movesComponent, origin, h);

      // Execute pathfinding
      simplePathfinding.FindCost(destination);
      if (!simplePathfinding.IsCostFound(destination))
      {
        std::cout << "failed to find agent with id = " << i << "\n";
        return 1;
      }

      ArrayType<Node<Area>> path;
      simplePathfinding.CollectPath(destination, path);
      ArrayType<Area> inaccessableParts;
      FromPathToFilledAreas(path, inaccessableParts);
      space->MakeAreasInaccessable(inaccessableParts);

      animation << "Agent " << i;
      animation << " " << inaccessableParts[0].point.x << " " << inaccessableParts[0].point.y << " " << std::setprecision(4) << inaccessableParts[0].interval.start;
      for (size_t i = 0; i + 1 < inaccessableParts.size(); i += 2)
      {
        Area currentArea = inaccessableParts[i];
        Area nextArea = inaccessableParts[i + 1];
        assert(nextArea.interval.IsValid());
        animation << " " << currentArea.point.x << " " << currentArea.point.y << " " << std::setprecision(4) << nextArea.interval.start;
        animation << " " << nextArea.point.x << " " << nextArea.point.y << " " << std::setprecision(4) << nextArea.interval.end;
      }
      animation << " " << inaccessableParts.back().point.x << " " << inaccessableParts.back().point.y << " " << std::setprecision(4) << inaccessableParts.back().interval.end;
      animation << "\n";

      std::cout << "success!\n";
    }

    return 0;
  }
};

int main()
{
  Mission mission(TEST_DATA_PATH "/empty-16-16-even-1.scen");

  if (mission.InitAnimation(TEST_DATA_PATH "/animation.txt"))
  {
    std::cout << "Animation init failed\n";
    return 1;
  }
  std::cout << "Animation init ok\n";


  if (mission.ReadSpace(300, TEST_DATA_PATH "/empty-16-16.map"))
  {
    std::cout << "ReadSpace failed\n";
    return 1;
  }
  std::cout << "ReadSpace ok\n";

  if (mission.InitAgents(51))
  {
    std::cout << "InitAgents failed\n";
    return 1;
  }
  std::cout << "InitAgents ok\n";


  if (mission.SolveCycle())
  {
    std::cout << "Planning failed\n";
    return 1;
  }
}
