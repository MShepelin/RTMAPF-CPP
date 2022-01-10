#pragma once

#include <inttypes.h>

using Access = bool;

#define ACCESSABLE true
#define INACCESSABLE false

using Cost = float;

using Time = float;

#define START_TIME 0.f

struct Point
{
  uint32_t x;
  uint32_t y;

  Point operator+(Point other)
  {
    return { x + other.x, y + other.y };
  }
};
