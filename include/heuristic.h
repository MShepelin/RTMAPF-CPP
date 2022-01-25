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

  virtual CellType GetOrigin() const { return CellType(); }

  virtual void SetOrigin(CellType origin) {}

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

// TODO create template
class SpaceAdapter : public Heuristic<Area>
{
  std::shared_ptr<Heuristic<Point>> heuristic;

public:
  SpaceAdapter(std::shared_ptr<Heuristic<Point>> inHeuristic)
    : heuristic(inHeuristic)
    , Heuristic<Area>(Area{ inHeuristic->GetOrigin() })
  {}

  virtual bool IsCostFound(Area to) const override { return heuristic->IsCostFound(to.point); };

  virtual Time GetCost(Area to) const override { return heuristic->GetCost(to.point); }

  virtual void FindCost(Area to) override { return heuristic->FindCost(to.point); };

  virtual Area GetOrigin() const override { return Area{ heuristic->GetOrigin() }; }

  virtual void SetOrigin(Area origin) override { heuristic->SetOrigin(origin.point); }
};
