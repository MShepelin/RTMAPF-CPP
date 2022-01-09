#include "segments.h"
#include <algorithm>
#include <cassert>

Segment Segment::operator&(const Segment& other) const
{
  Time newStart = std::max(start, other.start);
  Time newEnd = std::min(start, other.end);

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
