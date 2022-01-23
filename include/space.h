#pragma once

#include "search_types.h"
#include "segments.h"
#include <iostream>
#include <optional>
#include <stdexcept>

template<class CellType>
class Space
{
public:
  virtual Access GetAccess(CellType cell) const = 0;
  virtual void SetAccess(CellType cell, Access Access) = 0;
  virtual bool Contains(CellType cell) const = 0;

  virtual ~Space() {};
};

class space_error : public std::runtime_error
{
public:
  explicit space_error(const std::string& what_arg) : std::runtime_error(what_arg) {};
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

class SpaceReader
{
private:
  MapType<char, Access> symbolToAccess;

  inline bool CheckHogFileStart(std::istream& file, uint32_t& width, uint32_t& height);

public:
  SpaceReader();

  std::optional<RawSpace> FromHogFormat(std::istream& file);
};

class SegmentSpace : public Space<Area>
{
protected:
  MapType<Point, SegmentHolder> segmentGrid;

public:
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
private:
  Time depth;

public:
  SpaceTime(Time depth, const RawSpace& base);

  void MoveTime(Time deltaTime);

  Time GetDepth() const;

  // TODO override SetAccess to limit time by [0, depth]
};
