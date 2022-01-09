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

  /**
   * Intersection of two segments.
   */
  Segment operator&(const Segment& other) const;

  /**
   * If segments intersect returns a united segment.
   * Otherwise this function shouldn't be used (assertion).
   */
  Segment operator|(const Segment& other) const;

  bool operator<(const Segment& other) const;
};

class SegmentHolder
{
private:
  std::set<Segment> segments;

public:
  /**
   * If a new segment doesn't intersect with the stored segments
   * the new one will be added. Otherwise, a united segment will be created.
   */
  void AddSegment(Segment newSegment);
};
