#include "space.h"
#include <cassert>
#include <iostream>
#include <string>

Space::Space(uint32_t inWidth, uint32_t inHeight)
  : width(inWidth)
  , height(inHeight)
  , grid(inWidth * inHeight)
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

SpaceReader::SpaceReader()
  : symbolToAccess({ 
    {'@', INACCESSABLE}, 
    {'.', ACCESSABLE} })
{

}

std::optional<Space> SpaceReader::FromHogFormat(std::istream& file)
{
  uint32_t width = 0, height = 0;

  if (!CheckHogFileStart(file, width, height)) return {};

  Space readSpace(width, height);

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

    file.ignore('\n');
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
