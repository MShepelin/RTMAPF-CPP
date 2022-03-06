#pragma once

#include "nodes_heap.h"
#include "heuristic.h"
#include "search_types.h"
#include "moves.h"
#include <chrono>
#include <cassert>
#include <iostream>

template<typename CellType>
class SearchResult
{
private:
  std::chrono::steady_clock::time_point timer_start = std::chrono::high_resolution_clock::now();

  double time = 0;
  size_t nodescreated = 0;
  size_t numberofsteps = 0;

public:
  inline void IncrementSteps()
  {
    numberofsteps++;
  }

  inline void SetNodesCount(size_t inNodesCount)
  {
    nodescreated = inNodesCount;
  }

  inline void StartTimer()
  {
    timer_start = std::chrono::high_resolution_clock::now();
  }

  inline void StopTimer()
  {
    // TODO create timer object which incapsulates duration count like shared pointer

    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - timer_start;
    time += duration.count(); // in seconds
  }
};

template<typename CellType>
class Pathfinder : public Heuristic<CellType>
{
protected:
  using NodeType = Node<CellType>;
  using StatType = SearchResult<CellType>;

  mutable StatType statistics;

  NodesBinaryHeap<CellType> openNodes;
  MapType<CellType, NodeType> nodes;

  std::shared_ptr<Heuristic<CellType>> heuristic;
  std::shared_ptr<MoveComponent<CellType>> moves;

  virtual void TryToStopSearch(const NodeType& node, CellType searchDestination) {};

protected:
  void ExpandNode(NodeType& node);

public:
  Pathfinder(
    std::shared_ptr<MoveComponent<CellType>> inMoves, 
    CellType origin,
    std::shared_ptr<Heuristic<CellType>> inHeuristic);

  virtual bool IsCostFound(CellType to) const override;
  virtual Time GetCost(CellType to) const override;
  virtual void FindCost(CellType to) override;

  StatType GetStats() const { return statistics; }

  void CollectPath(CellType to, ArrayType<NodeType>& path, bool reverse = false) const;

  void SetHeuristic(std::shared_ptr<Heuristic<CellType>> inHeuristic);
};

template<typename CellType>
class WindowedPathfinder : public Pathfinder<CellType>
{
protected:
  Time depth;

protected:
  virtual void TryToStopSearch(const NodeType& node, CellType searchDestination) override
  {
    if (node.minTime >= depth)
    {
      nodes[searchDestination] = node;
    }
  }

public:
  WindowedPathfinder(
     std::shared_ptr<MoveComponent<CellType>> inMoves
    , CellType origin
    , std::shared_ptr<Heuristic<CellType>> inHeuristic
    , Time inDepth
  )
    : Pathfinder(inMoves, origin, inHeuristic)
    , depth(inDepth)
  {
    assert(depth > 0);
  }
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
void Pathfinder<CellType>::ExpandNode(NodeType& node)
{
  for (auto& validMove : moves->FindValidMoves(node))
  {
    const CellType& destination = validMove.destination;

    // Check if a potential node exists
    auto potential_node = nodes.find(destination);
    if (potential_node == nodes.end())
    {
      heuristic->FindCost(destination);
      if (heuristic->IsCostFound(destination)) 
      {
        continue;
      }

      // Create a new node.
      auto insert_result = nodes.insert({ 
          destination
        , NodeType(
            destination
          , node.minTime + validMove.waitCost + validMove.moveCost
          , heuristic->GetCost(destination)
          , validMove.moveCost
        )
      });

      NodeType& inserted_node = insert_result.first->second;
      openNodes.Insert(inserted_node);

      // Set the parential node.
      inserted_node.parent = &node;
    }
    else
    {
      Time nodeMinTime = node.minTime + validMove.waitCost + validMove.moveCost;
      if (potential_node->second.heursticToGoal >= 0 && potential_node->second.minTime > nodeMinTime)
      {
        openNodes.ImproveTime(potential_node->second, nodeMinTime);

        // Change the parential node to the one which is expanded.
        potential_node->second.parent = &node;
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
  statistics.StartTimer();

  while (!IsCostFound(to) && openNodes.Size())
  {
    statistics.IncrementSteps();

    NodeType& expanded_node = *openNodes.PopMin();
    expanded_node.MarkClosed();

    ExpandNode(expanded_node);
    TryToStopSearch(expanded_node, to);
  }

  statistics.SetNodesCount(nodes.size());
  statistics.StopTimer();
}

template<typename CellType>
void Pathfinder<CellType>::CollectPath(CellType to, ArrayType<NodeType>& path, bool reverse) const
{
  path.clear();

  if (!IsCostFound(to))
  {
    return;
  }

  statistics.StartTimer();

  const NodeType* currentNode = &nodes.at(to);
  while (currentNode)
  {
    path.push_back(NodeType(
        currentNode->cell
      , currentNode->minTime
      , currentNode->heursticToGoal
      , currentNode->arrivalCost
    ));
    currentNode = currentNode->parent;
  }

  if (!reverse)
  {
    std::reverse(path.begin(), path.end());
  }

  statistics.StopTimer();
}
