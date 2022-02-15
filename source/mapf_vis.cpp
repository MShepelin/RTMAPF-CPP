#include "pathfinder.h"
#include "space.h"
#include "hog2-utils/ScenarioLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>

class MovesTestSegment : public MoveComponent<Area>, public MoveComponent<Point>
{
protected:
  Time depth;
  ShapeSpace* space;
  ArrayType<Move<Point>> moves;

public:
  virtual ArrayType<Move<Area>> FindValidMoves(const Node<Area>& node) override
  {
    ArrayType<Move<Area>> result;
    Area origin = node.cell;

    Segment moveAvailable{ node.minTime, node.cell.interval.end };
    if (node.cell.interval.end >= depth)
    {
      result.push_back({ moveAvailable.GetLength(), Area{origin, {depth, depth}}, 0 });
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

        if (both.IsValid() && both.GetLength() >= move.cost)
        {
          Time overallCost = both.start + move.cost - node.minTime;
          result.push_back({ overallCost, Area{destinationPoint, segment}, move.cost });
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
      result.push_back({ move.cost, destinationPoint, move.cost });
    }

    return result;
  }

  MovesTestSegment(ArrayType<Move<Point>>& inmoves, ShapeSpace* inspace, Time inDepth)
    : space(inspace)
    , moves(inmoves)
    , depth(inDepth)
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
      Point start = { agent.GetStartX(), agent.GetStartY() };

      /*
      for (Point deltaPoint : agentShape.shape)
      {
        Point inaccessablePoint = start + deltaPoint;
        Segment inaccessableSegment = {0, depth};
        if (!space->ContainsSegmentsIn(inaccessablePoint) || !space->GetSegments(inaccessablePoint).Contains(inaccessableSegment))
        {
          std::cout << "failed to init agent with id = " << i << " (location is inaccessable)\n";
          return 1;
        }
        space->SetAccess({ inaccessablePoint, inaccessableSegment }, Access::Inaccessable);
      }*/
      if (!space->ContainsSegmentsIn(start))
      {
        std::cout << "failed to init agent with id = " << i << " (location is inaccessable)\n";
        return 1;
      }
      space->SetAccess({ start, {0, depth} }, Access::Inaccessable);
    }

    return 0;
  }

  void PrintAgentPath(int agentID, ArrayType<Node<Area>>& path)
  {
    animation << "Agent " << agentID << " " << agentPrintRad;

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

    animation << "\n";
    //Area lastCell = path.back().cell;
    //animation << " " << lastCell.point.x << " " << lastCell.point.y << " " << std::setprecision(4) << path.back().minTime << "\n";
    //animation << " " << lastCell.point.x << " " << lastCell.point.y << " " << std::setprecision(4) << lastCell.interval.end << "\n";

    std::cout << "success!\n";
  }

  int SolveCycle()
  {
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

    for (int i = 0; i < agentsNum; ++i)
    {
      // TODO add test when agent stands on one place
      std::cout << "Planning agent " << i << "... ";

      // Prepare agent
      Experiment agent = loader.GetNthExperiment(i);
      Point start = { agent.GetStartX(), agent.GetStartY() };
      Point goal = { agent.GetGoalX(), agent.GetGoalY() };
      Area origin = { start, {0, depth} };

      // Prepare agent space
      /*
      for (Point deltaPoint : agentShape.shape)
      {
        space->SetAccess({ start + deltaPoint, {0, depth} }, Access::Accessable);
      }*/

      space->SetAccess({ start, {0, depth} }, Access::Accessable);
      agentSpace = std::make_shared<ShapeSpace>(depth, space, agentShape);
      agentSpace->UpdateShape(start);
      agentSpace->UpdateShape(goal);

      // Prepare pathfinding
      std::shared_ptr<MovesTestSegment> movesComponent(new MovesTestSegment(moves, agentSpace.get(), depth));
      std::shared_ptr<EuclideanHeuristic> simpleHeurisitc(new EuclideanHeuristic(start));
      std::shared_ptr<Pathfinder<Point>> planeSearch(new Pathfinder<Point>(movesComponent, goal, simpleHeurisitc));
      std::shared_ptr<Heuristic<Area>> h(new SpaceAdapter<Point, Area>(planeSearch));
      WindowedPathfinder<Area> pathfinder(movesComponent, origin, h, depth);
      Area destination = Area::FromDepth(goal, depth);

      // Execute pathfinding
      pathfinder.FindCost(destination);
      if (!pathfinder.IsCostFound(destination))
      {
        std::cout << "failed to find agent with id = " << i << "\n";
        return 1;
      }

      ArrayType<Node<Area>> path;
      pathfinder.CollectPath(destination, path);

      ArrayType<Area> inaccessableParts;
      FromPathToFilledAreas(path, agentShape, inaccessableParts);
      space->MakeAreasInaccessable(inaccessableParts);

      PrintAgentPath(i, path);
    }

    return 0;
  }
};

int main()
{
  Mission mission(TEST_DATA_PATH "/ost003d-big-agents.scen");

  if (mission.InitAnimation(TEST_DATA_PATH "/animation.txt"))
  {
    std::cout << "Animation init failed\n";
    return 1;
  }
  std::cout << "Animation init ok\n";


  if (mission.ReadSpace(100, TEST_DATA_PATH "/ost003d.map"))
  {
    std::cout << "ReadSpace failed\n";
    return 1;
  }
  std::cout << "ReadSpace ok\n";


  if (mission.InitAgents(64))
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
