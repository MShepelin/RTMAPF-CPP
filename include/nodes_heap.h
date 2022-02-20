#pragma once

#include "search_types.h"
#include <cassert>

#define HEAP_START_CAPACITY 16

/**
 * Min heap for nodes.
 */
template<typename CellType>
class NodesBinaryHeap
{
public:
  using NodeType = Node<CellType>;

protected:
  // TODO not NodeType* but size_t, node can be moved in dynamic memory 
  // so we need to store ID, not pointer
  ArrayType<NodeType*> nodes;

  // TODO create NodesBinaryHeap.config
  bool isTieBreakMaxTime;

  void MoveUp(size_t nodeIndex);
  void MoveDown(size_t nodeIndex);

public:
  NodesBinaryHeap() = delete;
  NodesBinaryHeap(bool inIsTieBreakMaxTime);

  // Returns true if the first node is greater than the second one
  bool Compare(const NodeType& first, const NodeType& second) const;

  NodeType* PopMin();

  void Insert(NodeType& newNode);

  void ImproveTime(NodeType& changedNode, Time newMinTime);

  size_t Size() const;
};

template<typename CellType>
NodesBinaryHeap<CellType>::NodesBinaryHeap(bool inIsTieBreakMaxTime)
  : isTieBreakMaxTime(inIsTieBreakMaxTime)
  , nodes{ nullptr }
{
  nodes.reserve(HEAP_START_CAPACITY);
}

template<typename CellType>
void NodesBinaryHeap<CellType>::MoveUp(size_t nodeIndex)
{
  for (size_t parentIndex = (nodeIndex >> 1);
    parentIndex && Compare(*nodes[parentIndex], *nodes[nodeIndex]);
    nodeIndex >>= 1, parentIndex >>= 1)
  {
    std::swap(nodes[parentIndex]->heapIndex, nodes[nodeIndex]->heapIndex);
    std::swap(nodes[parentIndex], nodes[nodeIndex]);
  }
}

template<typename CellType>
void NodesBinaryHeap<CellType>::MoveDown(size_t nodeIndex)
{
  for (size_t minChildIndex = nodeIndex << 1; minChildIndex < nodes.size(); minChildIndex = nodeIndex << 1)
  {
    if (minChildIndex + 1 < nodes.size() && Compare(*nodes[minChildIndex], *nodes[minChildIndex + 1]))
    {
      ++minChildIndex;
    }

    NodeType& currentNode = *nodes[nodeIndex];
    NodeType& minChild = *nodes[minChildIndex];
    if (!Compare(currentNode, minChild))
    {
      return;
    }

    std::swap(currentNode.heapIndex, minChild.heapIndex);
    std::swap(nodes[nodeIndex], nodes[minChildIndex]);
    nodeIndex = minChildIndex;
  }
}

template<typename CellType>
void NodesBinaryHeap<CellType>::Insert(NodeType& newNode)
{
  newNode.heapIndex = nodes.size();
  nodes.emplace_back(&newNode);
  MoveUp(newNode.heapIndex);
}

template<typename CellType>
void NodesBinaryHeap<CellType>::ImproveTime(NodeType& changedNode, Time newMinTime)
{
  changedNode.minTime = newMinTime;
  MoveUp(changedNode.heapIndex);
}

template<typename CellType>
Node<CellType>* NodesBinaryHeap<CellType>::PopMin()
{
  if (Size() == 0)
  {
    return nullptr;
  }

  NodeType* result = nodes[1];
  std::swap(nodes[1], nodes.back());
  nodes.pop_back();

  if (Size() > 0)
  {
    nodes[1]->heapIndex = 1;
    MoveDown(1);
  }

  return result;
}

template<typename CellType>
size_t NodesBinaryHeap<CellType>::Size() const
{
  assert(nodes.size() >= 1);
  return nodes.size() - 1;
}

template<typename CellType>
bool NodesBinaryHeap<CellType>::Compare(const NodeType& first, const NodeType& second) const
{
  Time firstFullTime = first.minTime + first.heursticToGoal;
  Time secondFullTime = second.minTime + second.heursticToGoal;

  if (firstFullTime == secondFullTime)
  {
    return isTieBreakMaxTime == (first.minTime < second.minTime);
  }

  return firstFullTime > secondFullTime;
}
