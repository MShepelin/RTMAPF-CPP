#include "space.h"
#include "segments.h"
#include <fstream>
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

TEST(SegmentHolderTests, Addition)
{
  SegmentHolder segments;
  std::vector<Segment> answer = { {0, 2}, {3, 4}, {5, 6} };

  for (Segment s : answer)
  {
    segments.AddSegment(s);
  }

  auto iterator = segments.begin();
  for (size_t i = 0; i < answer.size(); ++i)
  {
    ASSERT_FALSE(iterator == segments.end());
    ASSERT_EQ(*(iterator++), answer[i]);
  }
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
