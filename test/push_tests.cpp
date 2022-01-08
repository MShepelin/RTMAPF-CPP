#include "search.h"
#include <gtest/gtest.h>

TEST(Baseline, CheckSum)
{
  int ans = Sum(2, 3);
  ASSERT_EQ(ans, 5);
}

int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
