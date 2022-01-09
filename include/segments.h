#pragma once

#include "search_types.h"
#include <set>

/**
 * Segment desribes time from the start to the end including both points.
 * If start <= end then segment is valid. 
 * Invalid segments should be considered empty.
 */
struct Segment
{
  Time start;
  Time end;

  bool IsValid() const;

  Segment operator&(const Segment& other) const;

  bool operator<(const Segment& other) const;
};

class SegmentHolder
{
  std::set<Segment> segments;
};