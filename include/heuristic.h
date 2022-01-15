#pragma once

#include "space.h"
#include "search_types.h"
#include "segments.h"
#include <memory>

class Heuristic
{
public:
  virtual Time GetCost(Area to) const = 0;

  virtual Time FindCost(Area to) = 0;

  virtual bool IsCostFound(Area to) const = 0;

  virtual ~Heuristic() {};
};
