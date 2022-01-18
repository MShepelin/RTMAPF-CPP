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
  // TODO add accessability?

  Time start;
  Time end;

  bool IsValid() const;
  static Segment Invalid();

  /**
   * Intersection of two segments.
   */
  Segment operator&(const Segment& other) const;

  /**
   * If segments intersect returns a united segment.
   * Otherwise this function shouldn't be used (assertion).
   */
  Segment operator|(const Segment& other) const;

  std::vector<Segment> operator-(const Segment& segment) const;

  bool operator<(const Segment& other) const;

  bool operator==(const Segment& other) const;
};

class SegmentHolder
{
private:
  std::set<Segment> segments;
  using const_iterator = std::set<Segment>::const_iterator;

public:
  SegmentHolder();
  SegmentHolder(Segment startSegment);

  /**
   * If a new segment doesn't intersect with the stored segments
   * the new one will be added. Otherwise, a united segment will be created.
   */
  void AddSegment(Segment newSegment);
  
  void RemoveSegment(Segment removal);

  SegmentHolder operator&(const SegmentHolder& other) const;

  const_iterator begin() const;
  const_iterator end() const;

  bool operator==(const SegmentHolder& other) const;
};

class Area
{
  Point point;
  Segment interval;
};
