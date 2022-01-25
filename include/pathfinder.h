#pragma once

#include "nodes_heap.h"
#include "heuristic.h"
#include "search_types.h"
#include "moves.h"
#include <cassert>

template<typename CellType>
struct SearchResult
{
  // TODO encapsulate timer

  size_t nodescreated = 0;
  size_t numberofsteps = 0;
  double time = 0;
};

template<typename CellType>
class Pathfinder : public Heuristic<CellType>
{
private:
  using NodeType = Node<CellType>;
  using StatType = SearchResult<CellType>;

  StatType statistics;

  NodesBinaryHeap<CellType> openNodes;
  MapType<CellType, NodeType> nodes;

  std::shared_ptr<Heuristic<CellType>> heuristic;
  std::shared_ptr<MoveComponent<CellType>> moves;

  void ExpandNode(NodeType* node);

  virtual void TryToStopSearch(const NodeType& node, CellType searchDestination) {};

public:
  Pathfinder(
    std::shared_ptr<MoveComponent<CellType>> inMoves, 
    CellType origin,
    std::shared_ptr<Heuristic<CellType>> inHeuristic);

  virtual Time GetCost(CellType to) const override;

  virtual void FindCost(CellType to) override;

  virtual bool IsCostFound(CellType to) const override;

  StatType GetStats() const
  {
    return statistics;
  }

  void CollectPath(CellType to, ArrayType<NodeType>& path) const;
};

template<typename CellType>
Pathfinder<CellType>::Pathfinder(
  std::shared_ptr<MoveComponent<CellType>> inMoves, 
  CellType origin,
  std::shared_ptr<Heuristic<CellType>> inHeuristic
  )
  : Heuristic(origin)
  , moves(inMoves)
  , openNodes(true)
  , heuristic(inHeuristic)
{ 
  heuristic->FindCost(origin);
  if (heuristic->IsCostFound(origin))
  {
    nodes[origin] = Node<CellType>(origin, Time(0), heuristic->GetCost(origin));
    openNodes.Insert(nodes[origin]);
  }
}

template<typename CellType>
void Pathfinder<CellType>::ExpandNode(NodeType* node)
{
  for (auto& validMove : moves->FindValidMoves(*node))
  {
    const CellType& destination = validMove.destination;
    const Time& cost = validMove.cost;

    // Check if a potential node exists
    auto potential_node = nodes.find(destination);
    if (potential_node == nodes.end())
    {
      heuristic->FindCost(destination);
      if (!heuristic->IsCostFound(destination))
      {
        continue;
      }

      // Create a new node.
      auto insert_result = nodes.insert({ 
        destination, 
        NodeType(
          destination, 
          node->minTime + cost,
          heuristic->GetCost(destination)
        )
      });

      NodeType& inserted_node = insert_result.first->second;
      openNodes.Insert(inserted_node);

      // Set the parential node.
      inserted_node.parent = node;
    }
    else
    {
      if (potential_node->second.heursticToGoal >= 0 && potential_node->second.minTime > node->minTime + cost)
      {
        openNodes.ImproveTime(potential_node->second, node->minTime + cost);

        // Change the parential node to the one which is expanded.
        potential_node->second.parent = node;
      }
      // If the potential node is in the close list, we never reopen/reexpand it.
    }
  }
}

template<typename CellType>
Time Pathfinder<CellType>::GetCost(CellType to) const
{
  assert(IsCostFound(to));

  return nodes.at(to).minTime;
}

template<typename CellType>
bool Pathfinder<CellType>::IsCostFound(CellType to) const
{
  return nodes.count(to) > 0;
}

template<typename CellType>
void Pathfinder<CellType>::FindCost(CellType to)
{
  if (IsCostFound(to))
  {
    return;
  }
  
  auto start = std::chrono::high_resolution_clock::now();

  while (openNodes.Size() && !IsCostFound(to))
  {
    statistics.numberofsteps++;

    // Remove expanded node from the heap.
    NodeType* expanded_node = openNodes.PopMin();

    // Expand the node.
    ExpandNode(expanded_node);

    // Mark expanded node as a node in the "close" list.
    expanded_node->heursticToGoal = -1;

    TryToStopSearch(*expanded_node, to);
  }

  statistics.nodescreated = nodes.size();
  std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
  statistics.time += duration.count();
}

template<typename CellType>
void Pathfinder<CellType>::CollectPath(CellType to, ArrayType<NodeType>& path) const
{
  path.clear();

  if (!IsCostFound(to))
  {
    return;
  }

  auto start = std::chrono::high_resolution_clock::now();

  const NodeType* currentNode = &nodes.at(to);
  while (currentNode)
  {
    path.push_back(NodeType(currentNode->cell, currentNode->minTime, currentNode->heursticToGoal));
    currentNode = currentNode->parent;
  }

  std::reverse(path.begin(), path.end());

  std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start;
  statistics.time += duration.count(); // in seconds
}
