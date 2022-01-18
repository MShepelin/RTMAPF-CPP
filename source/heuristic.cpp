#include "heuristic.h"
#include <algorithm>
#include <cmath>

EuclideanHeuristic::EuclideanHeuristic(Point inOrigin)
  : Heuristic(inOrigin)
  , origin(inOrigin)
{

}

Time EuclideanHeuristic::GetCost(Point to) const
{
  float deltaX = abs((float)origin.x - to.x);
  float deltaY = abs((float)origin.y - to.y);

  return std::sqrtf(deltaX * deltaX - deltaY * deltaY);
}

Time EuclideanHeuristic::FindCost(Point to)
{
  return GetCost(to);
}

bool EuclideanHeuristic::IsCostFound(Point to) const
{
  return true;
}
