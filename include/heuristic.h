#pragma once

#include "space.h"
#include "search_types.h"
#include "agent.h"
#include <memory>

template<class CellType>
class Heuristic
{
public:
  // TODO write comments to functions and desribe how 
  // IsCostFound should be used before 

  Heuristic() = delete;
  Heuristic(CellType origin) {};

  virtual bool IsCostFound(CellType to) const { return true; };

  virtual Time GetCost(CellType to) const { return 0.f; };

  virtual void FindCost(CellType to) { };

  virtual CellType GetOrigin() const { return CellType(); }

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

template<typename FromType, typename ToType>
class SpaceAdapter : public Heuristic<ToType>
{
  std::shared_ptr<Heuristic<FromType>> heuristic;

public:
  SpaceAdapter(std::shared_ptr<Heuristic<Point>> inHeuristic)
    : heuristic(inHeuristic)
    , Heuristic<ToType>(ToType(inHeuristic->GetOrigin()))
  {}

  virtual bool IsCostFound(ToType to) const override { return heuristic->IsCostFound(FromType(to)); }

  virtual Time GetCost(ToType to) const override { return heuristic->GetCost(FromType(to)); }

  virtual void FindCost(ToType to) override { return heuristic->FindCost(FromType(to)); }

  virtual ToType GetOrigin() const override { return ToType(heuristic->GetOrigin()); }
};
