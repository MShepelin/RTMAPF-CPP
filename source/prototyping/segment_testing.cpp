#include "segments.h"
#include <iostream>
#include <set>

void SegmentInteractiveTest()
{
  std::set<Segment> a;

  Time start, end;
  while ((std::cin >> start >> end) && !std::cin.fail())
  {
    a.insert({ start, end });
    for (auto& seg : a)
    {
      std::cout << "(" << seg.start << ":" << seg.end << ") ";
    }

    std::cout << "\n";
  }
}

int main()
{
  std::set<int> nums = { 1, 2, 3, 4 };

  return 0;
}