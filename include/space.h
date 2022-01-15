#pragma once

#include <vector>
#include "search_types.h"
#include "segments.h"
#include <iostream>
#include <optional>
#include <unordered_map>
#include <stdexcept>

template<class AccessType>
class Space
{
public:
  virtual const AccessType& GetAccess(Point point) const = 0;
  virtual void SetAccess(Point point, const AccessType& newAccess) = 0;
  virtual bool Contains(Point point) const = 0;

  virtual ~Space() {};
};

class space_error : public std::runtime_error
{
public:
  explicit space_error(const std::string& what_arg) : std::runtime_error(what_arg) {};
};

class RawSpace : public Space<Access>
{
private:
  std::vector<Access> grid;
  uint32_t width;
  uint32_t height;

private:
  inline size_t PointToIndex(Point& point) const;

public:
  RawSpace() = delete;
  RawSpace(uint32_t inWidth, uint32_t inHeight);

  const Access& GetAccess(Point point) const override;
  void SetAccess(Point point, const Access& newAccess) override;

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;

  bool Contains(Point point) const override;
};

class SpaceReader
{
private:
  std::unordered_map<char, Access> symbolToAccess;

  inline bool CheckHogFileStart(std::istream& file, uint32_t& width, uint32_t& height);

public:
  SpaceReader();

  std::optional<RawSpace> FromHogFormat(std::istream& file);
};

class SegmentSpace : public Space<SegmentHolder>
{
private:
  std::unordered_map<Point, SegmentHolder> segmentGrid;

public:
  SegmentSpace(Time depth, const RawSpace& base);

  const SegmentHolder& GetAccess(Point point) const;
  void SetAccess(Point point, const SegmentHolder& newAccess);

  bool Contains(Point point) const;
};
