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

  virtual Cost GetCost(Area to) const = 0;

  virtual Cost FindCost(Area to) = 0;

  virtual bool IsCostFound(Area to) const = 0;

  virtual ~Heuristic() {};
};
