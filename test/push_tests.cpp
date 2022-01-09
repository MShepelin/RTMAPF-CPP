#include "space.h"
#include "segments.h"
#include <fstream>
#include <sstream>
#include <gtest/gtest.h>

TEST(SpaceTests, Construction)
{
  Space space(3, 3);
  space.SetAccess({ 2, 2 }, INACCESSABLE);
  ASSERT_EQ(space.GetAccess({ 2, 2 }), INACCESSABLE);
}

TEST(SpaceTests, ReadHogFormat)
{
  SpaceReader reader;
  std::ifstream file(TEST_DATA_PATH "/chess_like.map");
  ASSERT_TRUE(file.is_open());

  std::optional<Space> space = reader.FromHogFormat(file);
  ASSERT_TRUE(space.has_value());

  uint32_t squareSize = 4;
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
  ASSERT_DEATH(g | a, "Assertion failed:.");
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
    SegmentHolder firstSegment;
    ReadSegments(input, firstSegment);

    SegmentHolder secondSegment;
    ReadSegments(input, secondSegment);

    SegmentHolder answer;
    ReadSegments(input, answer);

    ASSERT_EQ(firstSegment & secondSegment, answer);
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

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
