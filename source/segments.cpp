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
  return end < other.end;
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

std::vector<Segment> Segment::operator-(const Segment& segment) const
{
  if (!segment.IsValid())
  {
    return { *this };
  }

  Segment commonSegment = operator&(segment);

  std::vector<Segment> result;

  if (commonSegment.start > start)
  {
    result.push_back({ start, commonSegment.start });
  }
  if (commonSegment.end < end)
  {
    result.push_back({ commonSegment.end, end });
  }

  return result;
}

Segment Segment::Invalid()
{ 
  return Segment{ 1, -1 }; 
}

void SegmentHolder::AddSegment(Segment newSegment)
{
  const_iterator unionCandidate = segments.lower_bound({ newSegment.start, newSegment.start });
  while (unionCandidate != segments.end() && (newSegment & *unionCandidate).IsValid())
  {
    newSegment = newSegment | *unionCandidate;
    segments.erase(unionCandidate++);
  }

  segments.insert(newSegment);
}

void SegmentHolder::RemoveSegment(Segment removal)
{
  const_iterator removalCandidate = segments.upper_bound({ removal.start, removal.start });
  while (removalCandidate != segments.end() && (removal & *removalCandidate).IsValid())
  {
    auto difference = (*removalCandidate) - removal;
    segments.erase(removalCandidate++);
    for (auto& newSegment : difference)
    {
      segments.insert(newSegment);
    }
  }
}

SegmentHolder::const_iterator SegmentHolder::begin() const
{
  return segments.begin();
}

SegmentHolder::const_iterator SegmentHolder::end() const
{
  return segments.end();
}

SegmentHolder SegmentHolder::operator&(const SegmentHolder& other) const
{
  SegmentHolder newHolder;

  const_iterator selfSegment = begin();
  if (selfSegment == end())
  {
    return newHolder;
  }

  const_iterator otherSegment = other.segments.lower_bound({ selfSegment->start,  selfSegment->start });

  while (otherSegment != other.end() && selfSegment != end())
  {
    Segment newSegment = *selfSegment & *otherSegment;

    if (newSegment.IsValid())
    {
      newHolder.AddSegment(newSegment);
    }

    if (otherSegment->end > selfSegment->end)
    {
      selfSegment++;
    }
    else
    {
      otherSegment++;
    }
  }

  return newHolder;
}

bool SegmentHolder::operator==(const SegmentHolder& other) const
{
  return segments == other.segments;
}

SegmentHolder::SegmentHolder()
  : segments()
{

}

SegmentHolder::SegmentHolder(Segment startSegment)
  : segments({ startSegment })
{

}
