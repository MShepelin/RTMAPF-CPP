#pragma once

typedef bool Access;

#define ACCESSABLE true
#define INACCESSABLE false

struct Point
{
  int x;
  int y;

  Point operator+(Point other)
  {
    return { x + other.x, y + other.y };
  }
};
