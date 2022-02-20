#pragma once

#include "search_types.h"
#include "segments.h"
#include <iostream>
#include <optional>
#include <stdexcept>

template<typename CellType>
class Space
{
public:
  /**
   * If the Space doesn't contain a cell, this cell is inaccessable.
   * If the Space contains a cell, we can change it's access
   * (it can also become inaccessable).
   */
  virtual bool Contains(CellType cell) const = 0;

  /**
   * We can attempt to get an access of a cell only if the Space contains it.
   * Otherwise, the behavoir is undefined.
   */
  virtual Access GetAccess(CellType cell) const = 0;

  /**
   * We can attempt to set an access of a cell only if the Space contains it.
   * Otherwise, the behavoir is undefined.
   */
  virtual void SetAccess(CellType cell, Access Access) = 0;

  // In order to improve Space efficiency GetAccess and SetAccess don't throw exceptions.
  // Standard way to check correctness in run-time is to use assert for Debug.

  virtual ~Space() {};
};

class RawSpace : public Space<Point>
{
private:
  ArrayType<Access> grid;
  uint32_t width;
  uint32_t height;

private:
  inline size_t PointToIndex(Point& point) const;

public:
  RawSpace() = delete;
  RawSpace(uint32_t inWidth, uint32_t inHeight);

  Access GetAccess(Point point) const override;
  void SetAccess(Point point, Access newAccess) override;

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;

  bool Contains(Point point) const override;
};

class SegmentSpace : public Space<Area>
{
protected:
  MapType<Point, SegmentHolder> segmentGrid;

public:
  SegmentSpace();
  SegmentSpace(Time depth, const RawSpace& base);

  void SetSegments(Point point, const SegmentHolder& newAccess);
  const SegmentHolder& GetSegments(Point point) const;
  bool ContainsSegmentsIn(Point point) const;
  
  virtual Access GetAccess(Area cell) const override;
  virtual void SetAccess(Area cell, Access Access) override;
  virtual bool Contains(Area cell) const override;

  void MakeAreasInaccessable(const ArrayType<Area>& areas);
};

/**
 * Space that holds time segments limited by [0, depth]
 */
class SpaceTime : public SegmentSpace
{
protected:
  Time depth;

public:
  SpaceTime(Time depth);
  SpaceTime(Time depth, const RawSpace& base);

  void MoveTime(Time deltaTime);

  Time GetDepth() const;

  // TODO override SetAccess to limit time by [0, depth]
};

class SpaceReader
{
private:
  MapType<char, Access> symbolToAccess;

  inline bool CheckHogFileStart(std::istream& file, uint32_t& width, uint32_t& height);

public:
  SpaceReader();

  std::optional<RawSpace> FromHogFormat(std::istream& file);
};
