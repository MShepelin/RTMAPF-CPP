#pragma once

#include "nodes_heap.h"
#include "heuristic.h"
#include "search_types.h"
#include "moves.h"

template<typename CellType, typename SpaceType>
class Pathfinder : public Heuristic<CellType>
{
private:
  using NodeType = Node<CellType>;

  NodesBinaryHeap<CellType> openNodes;
  std::unordered_map<CellType, NodeType> nodes;

  std::shared_ptr<SpaceType> space;
  MoveComponent<CellType> moves;

  void ExpandNode(NodeType* node);

public:
  Pathfinder(std::shared_ptr<MoveComponent<CellType>> inMoves, CellType origin, std::shared_ptr<SpaceType> inSpace);
};

template<typename CellType, typename SpaceType>
Pathfinder<CellType, SpaceType>::Pathfinder(std::shared_ptr<MoveComponent<CellType>> inMoves, CellType origin, std::shared_ptr<SpaceType> inSpace)
  : Heuristic(origin)
  , moves(inMoves)
  , openNodes(true)
  , space(inSpace)
{ }

template<typename CellType, typename SpaceType>
void Pathfinder<CellType, SpaceType>::ExpandNode(NodeType* node)
{
  for (auto& validMove : moves.FindValidMoves(*node))
  {
    // Check if a potential node exists
    auto potential_node = nodes_.find(destination);
    if (potential_node == nodes_.end())
    {
      // Create a new node.
      auto insert_result = nodes_.insert({ destination, { destination, node->g + cost } });
      NodeType& inserted_node = insert_result.first->second;

      // Insert in the heap
      SetHeuristic(inserted_node);
      open_.Insert(inserted_node);

      // Set the parential node.
      inserted_node.parent = node;
    }
    else
    {
      if (potential_node->second.h >= 0 && potential_node->second.g > node->g + cost)
      {
        open_.DecreaseGValue(potential_node->second, node->g + cost);

        // Change the parential node to the one which is expanded.
        potential_node->second.parent = node;
      }
      // If the potential node is in the close list, we never reopen/reexpand it.
    }
  }
}
