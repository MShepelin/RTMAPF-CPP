#include "space.h"
#include <gtest/gtest.h>

TEST(Baseline, CheckSum)
{
  Space space(3, 3);
  space.SetAccess({ 2, 2 }, INACCESSABLE);
  ASSERT_EQ(space.GetAccess({ 2, 2 }), INACCESSABLE);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
