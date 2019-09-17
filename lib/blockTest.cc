#include <gtest/gtest.h>  // googletest header file
#include <iostream>
#include <string>
#include "blocking.h"
using std::string;
using namespace SEP::IO;

TEST(logicTest, blocking) {
  std::vector<int> blocksize(1, 4), nb(1, 10);
  //  EXPECT_DEATH(blocking(blocksize, nb), ".*failed.*");
  blocksize[0] = 5;
  ASSERT_NO_THROW(blocking(blocksize, nb));
}

TEST(blockAxis, blocking) {
  std::vector<int> blocksize(1, 5), nb(1, 10);
  blocking x = blocking(blocksize, nb);
  std::vector<int> nsz(1, 20);
  std::vector<std::vector<int>> blocks = x.blockAxis(nsz);
  EXPECT_EQ((int)blocks[0].size(), 2);
  for (int i = 0; i < 2; i++) {
    EXPECT_EQ((int)blocks[0][i], 10);
  }
  nsz[0] = 22;
  blocks = x.blockAxis(nsz);
  EXPECT_EQ((int)blocks[0].size(), 3);
  EXPECT_EQ((int)blocks[0][0], 10);
  EXPECT_EQ((int)blocks[0][1], 10);
  EXPECT_EQ((int)blocks[0][2], 2);

  nsz[0] = 27;
  blocks = x.blockAxis(nsz);
  EXPECT_EQ((int)blocks[0].size(), 3);
  EXPECT_EQ((int)blocks[0][0], 10);
  EXPECT_EQ((int)blocks[0][1], 10);
  EXPECT_EQ((int)blocks[0][2], 7);

  blocksize[0] = 2;
  x = blocking(blocksize, nb);

  nsz[0] = 22;
  blocks = x.blockAxis(nsz);
  EXPECT_EQ((int)blocks[0].size(), 3);
  EXPECT_EQ((int)blocks[0][0], 8);
  EXPECT_EQ((int)blocks[0][1], 8);
  EXPECT_EQ((int)blocks[0][2], 6);

  blocksize[0] = 5;
  nb[0] = 10;

  nsz[0] = 100;
  blocks = x.blockAxis(nsz);

  EXPECT_EQ((int)blocks[0].size(), 10);
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ((int)blocks[0][i], 10);
  }
}

TEST(makeBlocks, blocking) {
  std::vector<int> blocksize(1, 5), nb(1, 10), nsz(1, 22);
  blocking x = blocking(blocksize, nb);

  blockParams b = x.makeBlocks(nsz);

  EXPECT_EQ(b._nblocking[1], 3);
  EXPECT_EQ(b._fs[0][0], 0);
  EXPECT_EQ(b._fs[1][0], 10);
  EXPECT_EQ(b._fs[2][0], 20);
  EXPECT_EQ(b._ns[0][0], 10);
  EXPECT_EQ(b._ns[1][0], 10);
  EXPECT_EQ(b._ns[2][0], 2);
}
