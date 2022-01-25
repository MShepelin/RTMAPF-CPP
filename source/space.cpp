#include "space.h"
#include <cassert>
#include <iostream>
#include <string>
#include <algorithm>

RawSpace::RawSpace(uint32_t inWidth, uint32_t inHeight)
  : width(inWidth)
  , height(inHeight)
  , grid((size_t) inWidth * inHeight, INACCESSABLE)
{
}

Access RawSpace::GetAccess(Point point) const
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

void RawSpace::SetAccess(Point point, Access newAccess)
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
  return point.x >= 0 && point.x < (int) width && point.y >= 0 && point.y < (int) height;
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
  for (int row = 0; row < (int) height; ++row)
  {
    for (int column = 0; column < (int) width; ++column)
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

const SegmentHolder& SegmentSpace::GetSegments(Point point) const
{
  assert(ContainsSegmentsIn(point));
  return segmentGrid.at(point);
}

void SegmentSpace::SetSegments(Point point, const SegmentHolder & newAccess)
{ 
  segmentGrid[point] = newAccess;
}

bool SegmentSpace::ContainsSegmentsIn(Point point) const
{
  return segmentGrid.count(point) > 0;
}

SegmentSpace::SegmentSpace(Time depth, const RawSpace& base)
{
  assert(depth > 0);

  for (int x = 0; x < (int) base.GetWidth(); ++x)
  {
    for (int y = 0; y < (int) base.GetHeight(); ++y)
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
    if (!ContainsSegmentsIn(area.point))
    {
      continue;
    }

    segmentGrid[area.point].RemoveSegment(area.interval);

    // If segment holder becomes empty, it is still contained inside the SegmentSpace,
    // because in future it may be needed to add accessable intervals there
  }
}

Access SegmentSpace::GetAccess(Area cell) const
{
  assert(Contains(cell));

  return segmentGrid.at(cell.point).Contains(cell.interval) ? ACCESSABLE : INACCESSABLE;
}

void SegmentSpace::SetAccess(Area cell, Access Access)
{
  assert(Contains(cell));

  if (Access == ACCESSABLE)
  {
    segmentGrid.at(cell.point).AddSegment(cell.interval);
  }
  else if (Access == INACCESSABLE)
  {
    segmentGrid.at(cell.point).RemoveSegment(cell.interval);
  }
}

bool SegmentSpace::Contains(Area cell) const
{
  if (segmentGrid.count(cell.point) == 0)
  {
    return false;
  }

  return segmentGrid.at(cell.point).Contains(cell.interval);
}

SpaceTime::SpaceTime(Time inDepth, const RawSpace& base)
  : SegmentSpace(inDepth, base)\
  , depth(inDepth)
{

}

Time SpaceTime::GetDepth() const
{
  return depth;
}

void SpaceTime::MoveTime(Time deltaTime)
{
  assert(deltaTime >= 0);

  for (auto& [point, segment] : segmentGrid)
  {
    segment -= deltaTime;
    segment.RemoveSegment( {-deltaTime, 0} );
    segment.AddSegment({std::max(0.f, depth - deltaTime), depth});
  }
}
