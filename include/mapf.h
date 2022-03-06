#include "pathfinder.h"
#include "search_types.h"
#include "agent.h"
#include <memory>

class AdaptivePath
{
protected:
	std::shared_ptr<SpaceTime> space;
	std::vector<Node<Area>> reversedPath;
	Agent agent;
	Time depth;

public:
	AdaptivePath() = delete;
	AdaptivePath(Agent agent, std::shared_ptr<SpaceTime> inSpace, Time inDepth);

	bool Replan();

	std::vector<Node<Area>> GetPath() const;
};

class MultiagentPathfinder
{
protected:
	MapType<AgentID, AdaptivePath> idToPath;
	Time depth = 0;
	std::shared_ptr<SpaceTime> space;

public:
	MultiagentPathfinder(std::shared_ptr<SpaceTime> inSpace);

	Time GetAvailableMoveTime();

	void MoveTime(Time deltaTime);

	void AddAgent(Agent agent);

	void RemoveAgent(AgentID id);
};
