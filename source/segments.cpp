#include "segments.h"
#include <algorithm>
#include <cassert>

Segment Segment::operator&(const Segment& other) const
{
  Time newStart = std::max(start, other.start);
  Time newEnd = std::min(end, other.end);

  return Segment{ newStart , newEnd };
}

bool Segment::IsValid() const
{
  return start <= end;
}

bool Segment::operator<(const Segment& other) const
{
  assert(IsValid() && other.IsValid());
  return start < other.start;
}

Segment Segment::operator|(const Segment& other) const
{
  assert(operator&(other).IsValid());

  Time newStart = std::min(start, other.start);
  Time newEnd = std::max(end, other.end);

  return Segment{ newStart, newEnd };
}

bool Segment::operator==(const Segment& other) const
{
  return start == other.start && end == other.end;
}

void SegmentHolder::AddSegment(Segment newSegment)
{
  // TODO
}

SegmentHolder::const_iterator SegmentHolder::begin() const
{
  return segments.begin();
}

SegmentHolder::const_iterator SegmentHolder::end() const
{
  return segments.end();
}
