#pragma once

#include "space.h"
#include "search_types.h"
#include "agent.h"
#include <memory>

template<class CellType>
class Heuristic
{
public:
  Heuristic() = delete;
  Heuristic(CellType origin) {};

  virtual Time GetCost(CellType to) const { return 0.f; };

  virtual void FindCost(CellType to) { };

  virtual bool IsCostFound(CellType to) const { return true; };

  virtual ~Heuristic() {};
};

class EuclideanHeuristic : public Heuristic<Point>
{
private:
  Point origin;

public:
  EuclideanHeuristic(Point origin);

  virtual Time GetCost(Point to) const;

  virtual void FindCost(Point to);
};
