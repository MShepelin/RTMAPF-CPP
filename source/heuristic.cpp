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
  Time deltaX = abs((Time)origin.x - to.x);
  Time deltaY = abs((Time)origin.y - to.y);

  return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

void EuclideanHeuristic::FindCost(Point to)
{
  return;
}
