#pragma once

#include <vector>
#include "search_types.h"

class Space
{
private:
  std::vector<Access> grid;
  size_t width;
  size_t height;

private:
  inline size_t PointToIndex(Point& point) const;

public:
  Space() = delete;

  Space(size_t inWidth, size_t inHeight);

  Access GetAccess(Point point) const;

  void SetAccess(Point point, Access newAccess);
};
