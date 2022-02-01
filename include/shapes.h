#pragma once

#include "search_types.h"
#include "space.h"
#include "unordered_set"

struct Shape
{
  ArrayType<Point> shape;

  ArrayType<Point> ApplyShapeTo(Point point) const;
};

class ShapeSpace : public SpaceTime
{
private:
  std::shared_ptr<SegmentSpace> originalSpace;
  Shape shape;

  std::unordered_set<Area> pointCache;

public:
  ShapeSpace() = delete;
  ShapeSpace(Time depth, const RawSpace& base) = delete;
  ShapeSpace(Time depth, std::shared_ptr<SegmentSpace> inSpace, const Shape& inShape);

  void UpdateShape(Point point);
};
