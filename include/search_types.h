#pragma once

using Access = bool;

#define ACCESSABLE true
#define INACCESSABLE false

struct Point
{
  uint32_t x;
  uint32_t y;

  Point operator+(Point other)
  {
    return { x + other.x, y + other.y };
  }
};
