#include "pathfinder.h"
#include "space.h"
#include "hog2-utils/ScenarioLoader.h"

int main()
{
  /*
  for (int ID : agent_IDs)
  {
      auto plan = solver_.GetPlan(ID).lppath;
      if (!plan || plan->size() < 2) continue;
      for (int i = 1; i <= move_steps; ++i)
      {
          int cell_index = plan->size() - 1 - i;
          raw_paths_.at(ID).push_back(plan->at(cell_index).cell);
          raw_paths_.at(ID).back().t += start_time - 1;
      }
  }
  
  for (int ID : agent_IDs)
  {
      *log_stream << "Agent " << ID;
      for (SpaceTimeCell cell : raw_paths_.at(ID))
      {
          *log_stream << " " << cell.i << " " << cell.j << " " << cell.t;
      }

      *log_stream << "\n";
  }

  ****

  *log_stream << map.size

  for (int i = 0; i < map_.GetHeight(); ++i)
  {
      for (int j = 0; j < map_.GetWidth(); ++j)
      {
          if (!map_.IsCellTraversable(i, j))
          {
              *log_stream << "@";
          }
          else
          {
              *log_stream << ".";
          }
      }

      *log_stream << "\n";
  }


  */


}
