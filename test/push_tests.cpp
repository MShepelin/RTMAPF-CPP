#include "space.h"
#include <fstream>
#include <gtest/gtest.h>

TEST(Baseline, CheckSum)
{
  Space space(3, 3);
  space.SetAccess({ 2, 2 }, INACCESSABLE);
  ASSERT_EQ(space.GetAccess({ 2, 2 }), INACCESSABLE);
}

TEST(Baseline, ReadHogFormatMap)
{
  SpaceReader reader;
  std::ifstream file(TEST_DATA_PATH "/Berlin_1_256.map");
  ASSERT_TRUE(file.is_open());

  std::optional<Space> space = reader.FromHogFormat(file);
  ASSERT_TRUE(space.has_value());
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
