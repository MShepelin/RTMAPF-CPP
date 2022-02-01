#include "search_types.h"
#include "segments.h"

Point Point::operator+(Point other) const
{
  return { x + other.x, y + other.y };
}

bool Point::operator==(const Point& other) const
{
  return x == other.x && y == other.y;
}

Point::Point(int inX, int inY)
  : x(inX)
  , y(inY)
{ }

Point::Point(Area area)
  : x(area.point.x)
  , y(area.point.y)
{ }

Point::Point()
  : x(0)
  , y(0)
{ }
