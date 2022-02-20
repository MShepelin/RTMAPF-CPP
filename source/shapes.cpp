#include "shapes.h"

ArrayType<Point> Shape::ApplyShapeTo(Point point) const
{
  ArrayType<Point> result;
  for (const auto& shapePoint : shape)
  {
    result.push_back(shapePoint + point);
  }

  return result;
}

ShapeSpace::ShapeSpace(Time depth, std::shared_ptr<SegmentSpace> inSpace, const Shape& inShape)
  : SpaceTime(depth)
  , originalSpace(inSpace)
  , shape(inShape)
{ }

void ShapeSpace::UpdateShape(Point point)
{
  if (pointCache.count(point))
  {
    return;
  }

  pointCache.insert(point);

  ArrayType<Point> joinedPoints = shape.ApplyShapeTo(point);

  bool contains = true;
  for (Point& originalSpacePoint : joinedPoints)
  {
    if (!originalSpace->ContainsSegmentsIn(originalSpacePoint))
    {
      contains = false;
      break;
    }
  }

  if (!contains)
  {
    return;
  }

  segmentGrid[point] = SegmentHolder(Segment{ 0, depth });
  for (Point& originalSpacePoint : joinedPoints)
  {
    const SegmentHolder& segments = originalSpace->GetSegments(originalSpacePoint);
    segmentGrid[point] = segmentGrid[point] & segments;
  }
}

void FromPathToFilledAreas(const ArrayType<Node<Area>>& path, const Shape& shape, ArrayType<Area>& areas)
{
  areas.clear();

  for (size_t cellIndex = 0; cellIndex + 1 < path.size(); ++cellIndex)
  {
    Segment movementOnPlace{ path[cellIndex].minTime - path[cellIndex].arrivalCost, path[cellIndex + 1].minTime };

    for (const Point& deltaPoint : shape.shape)
    {
      Point spacePointFrom = path[cellIndex].cell.point + deltaPoint;
      areas.push_back(Area(spacePointFrom, movementOnPlace));
    }
  }

  Segment movementOnPlace{ path.back().minTime - path.back().arrivalCost, path.back().cell.interval.end };

  for (const Point& deltaPoint : shape.shape)
  {
    Point spacePointFrom = path.back().cell.point + deltaPoint;
    areas.push_back(Area(spacePointFrom, movementOnPlace));
  }
}
