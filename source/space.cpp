#include "space.h"
#include "assert.h"

Space::Space(size_t inWidth, size_t inHeight)
  : width(inWidth)
  , height(inHeight)
  , grid(inWidth*inHeight)
{
}

Access Space::GetAccess(Point point) const
{
  return grid[PointToIndex(point)];
}

size_t Space::PointToIndex(Point& point) const
{
  size_t index = point.x + point.y * width;
  assert(index < grid.size());
  return index;
}

void Space::SetAccess(Point point, Access newAccess)
{
  grid[PointToIndex(point)] = newAccess;
}
