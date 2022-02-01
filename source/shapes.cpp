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
