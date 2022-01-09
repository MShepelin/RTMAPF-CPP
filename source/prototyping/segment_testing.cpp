#include "segments.h"
#include <iostream>

int main()
{
  std::set<Segment> a;

  //std::string buf;
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

  return 0;
}