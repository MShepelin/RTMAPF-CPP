#pragma once

#include <inttypes.h>
#include <functional>
#include <vector>

// The difinitions of MAKE_HASHABLE and hash_combine are borrowed from:
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x

#define MAKE_HASHABLE(T, ...) \
    namespace std {\
        template<> struct hash<T> {\
            size_t operator()(const T &type) const {\
                size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

inline void hash_combine(std::size_t& seed) {};

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, rest...);
}

using Access = uint8_t;

#define ACCESSABLE 1
#define INACCESSABLE 0

using Time = float;

using Speed = float;

#define START_TIME 0.f

struct Point
{
  uint32_t x;
  uint32_t y;

  Point operator+(Point other)
  {
    return { x + other.x, y + other.y };
  }

  bool operator==(const Point& other) const
  {
    return x == other.x && y == other.y;
  }
};

MAKE_HASHABLE(Point, type.x, type.y);

using Shape = std::vector<Point>;

template<typename CellType>
struct Node
{
  CellType cell;
  // If heursticToGoal < 0 the node is in a "close" list, else it's in an "open" list
  // If minTime < 0 the node is currently inaccessable, 
  // else minTime = current min time to reach the node
  Time  minTime, heursticToGoal;
  size_t heapIndex;
  Node<CellType>* parent;

  Node(CellType inCell)
    : cell(inCell)
    , minTime(0)
    , heursticToGoal(-1)
    , parent(nullptr)
  { }

  Node(CellType inCell, Time inMinTime, Time inHeuristic = -1)
    : cell(inCell)
    , minTime(inMinTime)
    , heursticToGoal(inHeuristic)
    , parent(nullptr)
  { }

  Node()
    : cell()
    , minTime(-1)
    , heursticToGoal(-1)
    , parent(nullptr)
  { }
};
