#include "mapf.h"

AdaptivePath::AdaptivePath(Agent inAgent, std::shared_ptr<SpaceTime> inSpace, Time inDepth)
  : space(inSpace)
  , depth(inDepth)
  , agent(inAgent)
{ }

bool AdaptivePath::Replan()
{
  // Clear path
  if (reversedPath.size())
  {
    ArrayType<Area> inaccessableParts;
    FromReversedPathToFilledAreas(reversedPath, agent.shape, inaccessableParts);
    space->MakeAreasAccessable(inaccessableParts);
  }

  // Create agentSpace
  Point agentPoint = agent.GetCurrentPoint();
  std::shared_ptr<ShapeSpace> agentSpace = std::make_shared<ShapeSpace>(depth, space, agent.shape);
  agentSpace->UpdateShape(agentPoint);
  agentSpace->UpdateShape(agent.goal);

  if (!agentSpace->ContainsSegmentsIn(agentPoint) || 
    agentSpace->GetSegments(agentPoint).begin() == agentSpace->GetSegments(agentPoint).end())
  {
    std::cout << "failed to init agentSpace for an agent with id = " << agent.id << "\n";
    return false;
  }

  Area originalArea = { agentPoint, *agentSpace->GetSegments(agentPoint).begin() };

  // Prepare pathfinding
  std::shared_ptr<MovesTestSegment> movesComponent(new MovesTestSegment(agent.moves, agentSpace, depth));
  std::shared_ptr<EuclideanHeuristic> simpleHeurisitc(new EuclideanHeuristic(agentPoint));
  std::shared_ptr<Pathfinder<Point>> planeSearch(new Pathfinder<Point>(movesComponent, agent.goal, simpleHeurisitc));
  std::shared_ptr<Heuristic<Area>> h(new SpaceAdapter<Point, Area>(planeSearch));
  WindowedPathfinder<Area> pathfinder(movesComponent, originalArea, h, depth);
  Area destination = Area::FromDepth(agent.goal, depth);

  // Execute pathfinding
  pathfinder.FindCost(destination);
  if (!pathfinder.IsCostFound(destination))
  {
    std::shared_ptr<OneCellHeuristic<Point>> onePointHeuristic(new OneCellHeuristic<Point>(agentPoint));
    std::shared_ptr<Heuristic<Area>> adapter(new SpaceAdapter<Point, Area>(onePointHeuristic));
    pathfinder = WindowedPathfinder<Area>(movesComponent, originalArea, adapter, depth);

    pathfinder.FindCost(destination);
    if (!pathfinder.IsCostFound(destination))
    {
      std::cout << "failed to find agent with id = " << agent.id << "\n";
      return 1;
    }
  }

  pathfinder.CollectPath(destination, reversedPath, true);

  ArrayType<Area> inaccessableParts;
  FromReversedPathToFilledAreas(reversedPath, agent.shape, inaccessableParts);
  space->MakeAreasInaccessable(inaccessableParts);
  return true;
}

std::vector<Node<Area>> AdaptivePath::GetPath() const
{
  std::vector<Node<Area>> pathCopy = reversedPath;
  std::reverse(pathCopy.begin(), pathCopy.end());
  return pathCopy;
}

MultiagentPathfinder::MultiagentPathfinder(std::shared_ptr<SpaceTime> inSpace)
  : space(inSpace)
{

}

Time MultiagentPathfinder::GetAvailableMoveTime()
{
  return 0;
}

void MultiagentPathfinder::MoveTime(Time deltaTime)
{
	if (deltaTime > GetAvailableMoveTime()) {
		return;
	}

	// find id

}

void MultiagentPathfinder::AddAgent(Agent agent)
{
	if (idToPath.count(agent.id)) {
		// warning
	}
	//idToPath[agent.id] = AdaptivePath(agent);
	//idToPath[agent.id].Replan(depth);
}

void MultiagentPathfinder::RemoveAgent(AgentID id)
{
	if (!idToPath.count(id)) {
		return;
	}
	//idToPath.erase(id);
}
