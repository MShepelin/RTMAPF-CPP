#include "space.h"
#include <cassert>
#include <iostream>
#include <string>

RawSpace::RawSpace(uint32_t inWidth, uint32_t inHeight)
  : width(inWidth)
  , height(inHeight)
  , grid((size_t) inWidth * inHeight, INACCESSABLE)
{
}

const Access& RawSpace::GetAccess(Point point) const
{
  return grid[PointToIndex(point)];
}

size_t RawSpace::PointToIndex(Point& point) const
{
  // TODO throw exception?

  size_t index = point.x + (size_t) point.y * width;
  assert(index < grid.size());
  return index;
}

void RawSpace::SetAccess(Point point, const Access& newAccess)
{
  grid[PointToIndex(point)] = newAccess;
}

uint32_t RawSpace::GetWidth() const
{
  return width;
}

uint32_t RawSpace::GetHeight() const
{
  return height;
}

bool RawSpace::Contains(Point point) const
{
  // TODO incapsulate
  return point.x < width && point.y < height;
}

SpaceReader::SpaceReader()
  : symbolToAccess({ 
    {'@', INACCESSABLE}, 
    {'.', ACCESSABLE} })
{
}

std::optional<RawSpace> SpaceReader::FromHogFormat(std::istream& file)
{
  uint32_t width = 0, height = 0;

  if (!CheckHogFileStart(file, width, height)) return {};
  file.ignore(2, '\n');

  RawSpace readSpace(width, height);

  char grid_value;
  for (uint32_t row = 0; row < height; ++row)
  {
    for (uint32_t column = 0; column < width; ++column)
    {
      file.get(grid_value);
      Access newAccess = INACCESSABLE;
      if (symbolToAccess.count(grid_value))
      {
        newAccess = symbolToAccess[grid_value];
      }

      readSpace.SetAccess(Point{ column, row }, newAccess);
    }

    file.ignore(2, '\n');
  }

  return std::move(readSpace);
}

bool SpaceReader::CheckHogFileStart(std::istream& file, uint32_t& width, uint32_t& height)
{
  std::string buffer;
  std::string type;

  file >> buffer;
  if (buffer != "type")
  {
    std::cerr << "ReadSpace::FromHogFormat: cannot find \"type\" in file\n";
    return false;
  }

  file >> buffer;
  if (buffer != "octile")
  {
    std::cerr << "ReadSpace::FromHogFormat: type is not \"octile\"\n";
    return false;
  }

  file >> buffer >> height >> buffer >> width >> buffer;
  if (buffer != "map")
  {
    std::cerr << "ReadSpace::FromHogFormat: cannot find \"map\" in file\n";
    return false;
  }

  if (file.fail())
  {
    std::cerr << "ReadSpace::FromHogFormat: file read failed\n";
    return false;
  }

  return true;
}

const SegmentHolder& SegmentSpace::GetAccess(Point point) const
{
  return segmentGrid.at(point);
}

void SegmentSpace::SetAccess(Point point, const SegmentHolder & newAccess)
{ 
  segmentGrid[point] = newAccess;
}

bool SegmentSpace::Contains(Point point) const
{
  return segmentGrid.count(point) > 0;
}

SegmentSpace::SegmentSpace(Time depth, const RawSpace& base)
{
  assert(depth > 0);

  for (uint32_t x = 0; x < base.GetWidth(); ++x)
  {
    for (uint32_t y = 0; y < base.GetHeight(); ++y)
    {
      Point point = { x, y };

      if (base.GetAccess(point) == ACCESSABLE)
      {
        segmentGrid[point] = SegmentHolder(Segment{0, depth});
      }
    }
  }
}

void SegmentSpace::MakeAreasInaccessable(const std::vector<Area>& areas)
{
  for (const Area& area : areas)
  {
    if (!Contains(area.point))
    {
      continue;
    }

    segmentGrid[area.point].RemoveSegment(area.interval);

    // If segment holder becomes empty, it is still contained inside the SegmentSpace,
    // because in future it may be needed to add accessable intervals there
  }
}
