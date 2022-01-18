#include "space.h"
#include "segments.h"
#include "agent.h"
#include <fstream>
#include <sstream>
#include "nodes_heap.h"
#include <gtest/gtest.h>

TEST(SpaceTests, Construction)
{
  RawSpace space(3, 3);
  space.SetAccess({ 2, 2 }, ACCESSABLE);
  ASSERT_EQ(space.GetAccess({ 2, 2 }), ACCESSABLE);
  ASSERT_EQ(space.GetAccess({ 1, 1 }), INACCESSABLE);
  ASSERT_EQ(space.GetHeight(), 3);
  ASSERT_EQ(space.GetWidth(), 3);
}

TEST(SpaceTests, ReadHogFormat)
{
  SpaceReader reader;
  std::ifstream file(TEST_DATA_PATH "/chess_like.map");
  ASSERT_TRUE(file.is_open());

  std::optional<RawSpace> space = reader.FromHogFormat(file);
  ASSERT_TRUE(space.has_value());

  uint32_t squareSize = 4;
  ASSERT_EQ(space.value().GetHeight(), 4);
  ASSERT_EQ(space.value().GetWidth(), 4);
  for (uint32_t i = 0; i < squareSize; ++i)
  {
    for (uint32_t j = 0; j < squareSize; ++j)
    {
      // Conversation from location to Access
      Access correctAccess = (i + j + 1) % 2;
      Point point{ i, j };
      ASSERT_EQ(space.value().GetAccess(point), correctAccess);
    }
  }
}

TEST(SpaceTests, SegmentSpaceConstruction)
{
  Time depth = 3;

  RawSpace space(3, 3);
  space.SetAccess({ 2, 2 }, ACCESSABLE);

  SegmentSpace test(depth, space);

  ASSERT_TRUE(test.Contains({ 2, 2 }));
  ASSERT_FALSE(test.Contains({ 1, 1 }));

  Segment ans = Segment{ 0, 3 };
  ASSERT_EQ(test.GetAccess({ 2, 2 }), ans);

  SegmentHolder newHolder({ -1, 4 });
  test.SetAccess({ 1, 1 }, newHolder);
  ASSERT_EQ(test.GetAccess({ 1, 1 }), newHolder);
}

TEST(SpaceTests, MakeAreasInaccessable)
{
  Time depth = 3;
  RawSpace space(3, 3);
  space.SetAccess({ 2, 2 }, ACCESSABLE);
  space.SetAccess({ 0, 0 }, ACCESSABLE);
  SegmentSpace test(depth, space);

  std::vector<Area> removeAreas = { 
    Area{{0, 0}, {1, 2}}, 
    Area{{2, 2}, {2, 5}},
    Area{{1, 1}, {1, 2}} 
  };
  test.MakeAreasInaccessable(removeAreas);

  SegmentHolder result1;
  result1.AddSegment({ 0, 1 });
  result1.AddSegment({ 2, 3 });

  SegmentHolder result2;
  result2.AddSegment({ 0, 2 });

  ASSERT_TRUE(test.Contains({ 2, 2 }));
  ASSERT_TRUE(test.Contains({ 0, 0 }));
  ASSERT_FALSE(test.Contains({ 1, 1 }));

  ASSERT_EQ(test.GetAccess({ 0, 0 }), result1);
  ASSERT_EQ(test.GetAccess({ 2, 2 }), result2);
}

TEST(SpaceTimeTests, MoveTime)
{
  Time depth = 3;
  RawSpace space(3, 3);
  space.SetAccess({ 2, 2 }, ACCESSABLE);
  space.SetAccess({ 0, 0 }, ACCESSABLE);
  SpaceTime spaceTime(depth, space);

  std::vector<Area> removeAreas = {
    Area{{0, 0}, {1, 2}},
    Area{{2, 2}, {2, 5}},
    Area{{1, 1}, {1, 2}}
  };
  spaceTime.MakeAreasInaccessable(removeAreas);

  spaceTime.MoveTime(2);

  SegmentHolder result1;
  result1.AddSegment({ 0, 3 });

  SegmentHolder result2;
  result2.AddSegment({ 1, 3 });

  ASSERT_EQ(spaceTime.GetAccess({ 0, 0 }), result1);
  ASSERT_EQ(spaceTime.GetAccess({ 2, 2 }), result2);
}

TEST(SegmentsTests, Intersection)
{
  Segment b{ 0, 10 };
  Segment c{ 2, 8 };
  ASSERT_EQ(c, c & b);
  ASSERT_EQ(c, b & c);
  ASSERT_EQ(c, c);

  Segment a{ -5, 5 };
  Segment d{ 0, 5 };
  ASSERT_EQ(d, a & b);
  ASSERT_EQ(d, b & a);
}

TEST(SegmentsTests, Union)
{
  Segment a{ -5, 5 };
  Segment b{ 5, 10 };
  Segment c{ -5, 10 };
  ASSERT_EQ(a | b, c);
  ASSERT_EQ(b | a, c);

  Segment g{ 10, 15 };
  ASSERT_FALSE((g & a).IsValid());
  ASSERT_DEATH(g | a, ".*Assertion.*failed.*");
}

TEST(SegmentsTests, Comparison)
{
  Segment a{ 2, 3 };
  Segment c{ 1, 4 };
  Segment b{ 4, 5 };
  ASSERT_TRUE(a < b);
  ASSERT_TRUE(a < c);
  ASSERT_TRUE(c < b);
  ASSERT_FALSE(b < a);
}

TEST(SegmentsTests, Substraction)
{
  Segment a{ 2, 5 };
  Segment b{ 3, 4 };
  Segment c{ 1, 3 };
  Segment d{ 4, 8 };

  std::vector<Segment> res = { {2, 3}, {4, 5} };
  ASSERT_EQ(a - b, res);
  ASSERT_EQ((b - a).size(), 0);

  std::vector<Segment> res2 = { { 3, 5 } };
  ASSERT_EQ(a - c, res2);

  std::vector<Segment> res3 = { { 1, 2 } };
  ASSERT_EQ(c - a, res3);

  ASSERT_EQ(b - d, std::vector<Segment>{ b });
  ASSERT_EQ(c - Segment::Invalid(), std::vector<Segment>{ c });
}

void SegmentsAdditionWithCheck(SegmentHolder& segments, const std::vector<Segment>& answer, const std::vector<Segment>& input)
{
  for (Segment s : input)
  {
    segments.AddSegment(s);
  }

  auto iterator = segments.begin();
  for (size_t i = 0; i < answer.size(); ++i)
  {
    ASSERT_FALSE(iterator == segments.end());
    ASSERT_EQ(*iterator, answer[i]) << "result: " << iterator->start << "-" << iterator->end \
      << " expected: " << answer[i].start << "-" << answer[i].end << "\n";
    iterator++;
  }
}

TEST(SegmentHolderTests, SeparateSegmentsAddition)
{
  std::vector<Segment> answer = { {0, 2}, {3, 4}, {5, 6} };

  for (std::vector<Segment> input : std::vector<std::vector<Segment>>{
    { {0, 2}, {3, 4}, {5, 6} },
    { {5, 6}, {3, 4}, {0, 2} },
    { {5, 6}, {0, 2}, {3, 4} } })
  {
    SegmentHolder segments;
    SegmentsAdditionWithCheck(segments, answer, input);
  }
}

TEST(SegmentHolderTests, IntersectingSegmentsAddition)
{
  SegmentHolder segments;
  std::vector<Segment> initInput = { {0, 2}, {3, 4}, {5, 8} };
  SegmentsAdditionWithCheck(segments, initInput, initInput);

  for (auto [answer, input] : std::vector<std::pair<std::vector<Segment>, std::vector<Segment>>>{
      { {{0, 2}, {3, 4}, {5, 10} }, { {   6, 10  } } },
      { {{-5, 2}, {3, 4}, {5, 8} }, { {  -5, 1   } } },
      { {{0, 2}, {3, 4.5}, {5, 8}}, { { 3.5, 4.5 } } }
    })
  {
    SegmentHolder segmentsCopy = segments;
    SegmentsAdditionWithCheck(segmentsCopy, answer, input);
  }
}

TEST(SegmentHolderTests, SegmentsAdditionMixed)
{
  SegmentHolder segments;
  std::vector<Segment> initInput = { {0, 2}, {4, 6}, {8, 10}, {12, 14} };
  SegmentsAdditionWithCheck(segments, initInput, initInput);

  for (auto [answer, input] : std::vector<std::pair<std::vector<Segment>, std::vector<Segment>>>{
      { {{0, 6}, {7, 15} }, {  {7, 15}, {1, 5} } },
      { {{-5, 2}, {4, 14}, {16, 20} }, { {16, 20}, {-5, 1}, {6, 12} } },
      { {{-20, 20} }, { {-3, 3}, {-20, 20}, {4, 8} } }
    })
  {
    SegmentHolder segmentsCopy = segments;
    SegmentsAdditionWithCheck(segmentsCopy, answer, input);
  }
}

void ReadSegments(std::istream& input, SegmentHolder& segments)
{
  size_t nSegments = 0;
  input >> nSegments;
  ASSERT_FALSE(input.fail());
  ASSERT_EQ(nSegments % 2, 0);

  for (size_t i = 0; i < nSegments / 2; ++i)
  {
    Time start, end;
    input >> start >> end;
    segments.AddSegment({ start, end });
  }
}

void IntersectSegments(std::istream& input)
{
  while (!input.eof())
  {
    SegmentHolder firstSegmentHolder;
    ReadSegments(input, firstSegmentHolder);

    SegmentHolder secondSegmentHolder;
    ReadSegments(input, secondSegmentHolder);

    SegmentHolder answer;
    ReadSegments(input, answer);

    ASSERT_EQ(firstSegmentHolder & secondSegmentHolder, answer);
  }
}

TEST(SegmentHolderTests, Intersection)
{
  std::stringstream input(\
    "8 0 2 4 6 8 10 13 14\n"\
    "4 3 5 8 12\n"\
    "4 4 5 8 10\n"\
    \
    "8 0 8 13 14 15 16 17 20\n"\
    "8 2 10 12 18 19 21 22 23\n"\
    "10 2 8 13 14 15 16 17 18 19 20\n"\
    \
    "4 1 2 3 4\n"\
    "6 4 5 6 7 8 9\n"\
    "2 4 4\n"
    \
    "2 1 10\n"\
    "12 2 3 4 5 6 7 8 9 11 12 13 14\n"\
    "8 2 3 4 5 6 7 8 9");

  IntersectSegments(input);
}

void RemoveSegmentTest(std::istream& input)
{
  while (!input.eof())
  {
    SegmentHolder firstSegmentHolder;
    ReadSegments(input, firstSegmentHolder);

    Time start, end;
    input >> start >> end;
    Segment removal{ start, end };

    SegmentHolder answer;
    ReadSegments(input, answer);

    firstSegmentHolder.RemoveSegment(removal);
    ASSERT_EQ(firstSegmentHolder, answer);
  }
}

TEST(SegmentHolderTests, SegmentRemoval)
{
  std::stringstream input(\
    "6 2 3 4 5 6 7\n"\
    "0 1\n"\
    "6 2 3 4 5 6 7\n"\
    \
    "6 2 3 4 5 6 7\n"\
    "8 9\n"\
    "6 2 3 4 5 6 7\n"\
    \
    "6 0 2 3 4 5 7\n"\
    "1 6\n"\
    "4 0 1 6 7");

  RemoveSegmentTest(input);
}

TEST(SegmentHolderTests, Reduction)
{
  SegmentHolder segments;
  segments.AddSegment({ 2, 3 });
  segments.AddSegment({ -1, 1 });
  segments.AddSegment({ 4, 5 });
  segments.AddSegment({ 6, 8 });

  segments -= 3;

  SegmentHolder answer;
  answer.AddSegment({ -1, 0 });
  answer.AddSegment({ -4, -2 });
  answer.AddSegment({ 1, 2 });
  answer.AddSegment({ 3, 5 });

  ASSERT_EQ(segments, answer);
}

TEST(AgentTest, MakeAgentSpace)
{
  // TODO remove simplification
  Agent agent{ 2 };

  Time depth = 3;

  RawSpace baseSpace(3, 3);
  baseSpace.SetAccess({ 2, 2 }, ACCESSABLE);

  SegmentSpace space(depth, baseSpace);

  SegmentSpace newSpace = AgentOperations::MakeSpaceFromAgentShape(space, agent);

  for (int x = 0; x < 4; ++x)
  {
    for (int y = 0; y < 4; ++y)
    {
      Point point{ x, y };
      ASSERT_EQ(newSpace.Contains(point), space.Contains(point));
      if (!newSpace.Contains(point)) continue;

      ASSERT_EQ(newSpace.GetAccess(point), space.GetAccess(point));
    }
  }
}

TEST(NodesBinaryHeap, CheckTies)
{
  NodesBinaryHeap<int> heap(true);
  std::vector<Node<int>> check = {
      { 8, 10, 5 },
      { 3, 3, 3 },
      { 6, 0, 8 },
      { 1, 3, 2 },
      { 0, 1, 0 },
      { 2, 2, 3 },
      { 9, 5, 10 },
      { 7, 9, 1 },
      { 5, 4, 4 },
      { 4, 5, 3 }
  };

  for (auto& i : check)
  {
    heap.Insert(i);
  }

  int counter = 0;
  while (heap.Size())
  {
    Node<int>* node = heap.PopMin();
    EXPECT_EQ(node->cell, counter);
    counter++;
  }
}


int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
