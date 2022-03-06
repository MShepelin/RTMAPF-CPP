#include "pathfinder.h"
#include "space.h"
#include "hog2-utils/ScenarioLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>
#include "agent.h"
#include "mapf.h"

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

      for (Point deltaPoint : agentShape.shape)
      {
        Point inaccessablePoint = start + deltaPoint;
        Segment inaccessableSegment = {0, depth};
        if (!space->ContainsSegmentsIn(inaccessablePoint) || !space->GetSegments(inaccessablePoint).Contains(inaccessableSegment))
        {
          std::cout << "failed to init agent with id = " << i << " (location " << inaccessablePoint.x << ";" << inaccessablePoint.y << " is inaccessable)\n";
          return 1;
        }
        space->SetAccess({ inaccessablePoint, inaccessableSegment }, Access::Inaccessable);
      }
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
      if (finishWaiting > path[i].minTime + 1e-5)
      {
        animation << " " << currentArea.point.x << " " << currentArea.point.y << " " << std::setprecision(4) << finishWaiting;
      }
    }

    animation << "\n";
    std::cout << "success!\n";
  }

  int SolveCycle()
  {
    for (int i = 0; i < agentsNum; ++i)
    {
      std::cout << "Planning agent " << i << "... ";
      
      Experiment experiment = loader.GetNthExperiment(i);
      Agent agent;
      agent.id = i;
      agent.shape = agentShape;
      agent.currentX = experiment.GetStartX();
      agent.currentY = experiment.GetStartY();
      agent.goal = { experiment.GetGoalX(), experiment.GetGoalY() };

      Point start = { experiment.GetStartX(), experiment.GetStartY() };
      for (Point deltaPoint : agentShape.shape)
      {
        Point inaccessablePoint = start + deltaPoint;
        Segment inaccessableSegment = { 0, depth };
        space->SetAccess({ inaccessablePoint, inaccessableSegment }, Access::Accessable);
      }

      AdaptivePath adaptivePath(agent, space, depth);
      if (!adaptivePath.Replan()) return 1;
      std::vector<Node<Area>> path = adaptivePath.GetPath();
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
