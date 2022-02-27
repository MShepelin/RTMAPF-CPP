#pragma once

#include "search_types.h"
#include "space.h"

template<typename CellType>
struct Move
{
  Time moveCost;
  CellType destination;
  Time waitCost = 0;
};

template<typename CellType>
class MoveComponent
{
public:
  virtual ArrayType<Move<CellType>> FindValidMoves(const Node<CellType>& node) = 0;

  virtual ~MoveComponent() {};
};
