#pragma once

#include "nodes_heap.h"
#include "heuristic.h"
#include "search_types.h"

template<typename AccessType, typename CellType>
class Pathfinder : public Heuristic<CellType>
{
private:
  using NodeType = Node<CellType>;
  using SpaceType = Space<AccessType, CellType>;

  ArrayType<Move> moves;
  NodesBinaryHeap<CellType> openNodes;
  std::unordered_map<CellType, NodeType> nodes;
  std::shared_ptr<SpaceType> space;

public:
  Pathfinder(const ArrayType<Move>& inMoves, CellType origin, std::shared_ptr<SpaceType> inSpace);
};

template<typename AccessType, typename CellType>
Pathfinder<AccessType, CellType>::Pathfinder(const ArrayType<Move>& inMoves, CellType origin, std::shared_ptr<SpaceType> inSpace)
  : Heuristic(origin)
  , moves(inMoves)
  , openNodes(true)
{ }
