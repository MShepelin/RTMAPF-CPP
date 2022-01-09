#pragma once

#include <vector>
#include "search_types.h"
#include <iostream>
#include <optional>
#include <unordered_map>


class Space
{
private:
  std::vector<Access> grid;
  uint32_t width;
  uint32_t height;

private:
  inline size_t PointToIndex(Point& point) const;

public:
  Space() = delete;
  Space(uint32_t inWidth, uint32_t inHeight);

  Access GetAccess(Point point) const;
  void SetAccess(Point point, Access newAccess);

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;
};

class SpaceReader
{
private:
  std::unordered_map<char, Access> symbolToAccess;

  inline bool CheckHogFileStart(std::istream& file, uint32_t& width, uint32_t& height);

public:
  SpaceReader();

  std::optional<Space> FromHogFormat(std::istream& file);
};
