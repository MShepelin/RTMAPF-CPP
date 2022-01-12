#pragma once

#include "space.h"
#include "search_types.h"
#include "segments.h"
#include <memory>

class Heuristic
{
protected:
  std::weak_ptr<Space> space;

public:
  Heuristic() = delete;

  Heuristic(std::weak_ptr<Space> inSpace);

  virtual Time GetCost(Area to) const = 0;

  virtual Time FindCost(Area to) = 0;

  virtual bool IsCostFound(Area to) const = 0;

  virtual ~Heuristic() {};
};
