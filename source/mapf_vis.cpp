#include "pathfinder.h"
#include "space.h"
#include "hog2-utils/ScenarioLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

class MovesTestSegment : public MoveComponent<Area>
{
protected:
  ShapeSpace* space;
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

      space->UpdateShape(destinationPoint);
      if (!space->ContainsSegmentsIn(destinationPoint)) continue;
      const SegmentHolder& segHolder = space->GetSegments(destinationPoint);

      for (Segment segment : segHolder)
      {
        Segment both = moveAvailable & segment;

        if (both.IsValid() && both.GetLength() >= move.cost)
        {
          Time overallCost = both.start + move.cost - node.minTime;
          result.push_back({ overallCost, Area{destinationPoint, segment}, move.cost });
        }
      }
    }

    return result;
  }

  MovesTestSegment(ArrayType<Move<Point>>& inmoves, ShapeSpace* inspace)
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

  std::shared_ptr<ShapeSpace> agentSpace;
  Shape agentShape = { ArrayType<Point>{ {0, 0}, {0, 1}, {0, -1}, {1, 0}, {-1, 0} } };
  double agentPrintRad = 1;

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
    agentSpace = std::make_shared<ShapeSpace>(inDepth, space, agentShape);

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
      Area origin = { start, {0, depth} };

      space->SetAccess(origin, Access::Accessable);
      agentSpace = std::make_shared<ShapeSpace>(depth, space, agentShape);
      agentSpace->UpdateShape(start);
      agentSpace->UpdateShape(goal);

      const SegmentHolder& segHolder = agentSpace->GetSegments(goal);
      if (segHolder.end() == segHolder.begin())
      {
        std::cout << "One agent sits on the goal of another\n";
        return 1;
      }

      const auto iter = --segHolder.end();
      Area destination = { goal, *iter };

      // Prepare pathfinding
      ArrayType<Move<Point>> moves =
      {
        Move<Point>{ 1, {0, 1}},
        Move<Point>{ 1, {0, -1}},
        Move<Point>{ 1, {1, 0}},
        Move<Point>{ 1, {-1, 0}},
      };

      
      std::shared_ptr<MovesTestSegment> movesComponent(new MovesTestSegment(moves, agentSpace.get()));

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
      FromPathToFilledAreas(path, agentShape, inaccessableParts);
      space->MakeAreasInaccessable(inaccessableParts);

      animation << "Agent " << i << " " << agentPrintRad;

      Area firstCell = path[0].cell;
      for (size_t i = 0; i + 1 < path.size(); ++i)
      {
        Area currentArea = path[i].cell;
        animation << " " << currentArea.point.x << " " << currentArea.point.y << " " << std::setprecision(4) << path[i].minTime;

        Time finishWaiting = path[i + 1].minTime - path[i + 1].arrivalCost;
        if (finishWaiting > path[i].minTime)
        {
          animation << " " << currentArea.point.x << " " << currentArea.point.y << " " << std::setprecision(4) << path[i + 1].minTime - path[i + 1].arrivalCost;
        }
      }
      Area lastCell = path.back().cell;
      animation << " " << lastCell.point.x << " " << lastCell.point.y << " " << std::setprecision(4) << path.back().minTime;
      animation << " " << lastCell.point.x << " " << lastCell.point.y << " " << std::setprecision(4) << lastCell.interval.end << "\n";

      std::cout << "success!\n";
    }

    return 0;
  }
};

int main()
{
  Mission mission(TEST_DATA_PATH "/empty-16-16-big-agents.scen");

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


  if (mission.InitAgents(5))
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
