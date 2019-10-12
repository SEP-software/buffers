#include <gtest/gtest.h>  // googletest header file
#include <iostream>
#include <string>
#include "block.h"
using std::string;
using namespace SEP::blocking;

class data4D {
 public:
  data4D(const int n1, const int n2, const int n3, const int n4) {
    nb.resize(7);
    vec.resize(n1 * n2 * n3 * n4);
    nb[0] = 1;
    nb[1] = n1;
    nb[2] = n1 * n2;
    nb[3] = n1 * n2 * n3;
    nb[4] = nb[5] = nb[6] = n1 * n2 * n3 * n4;
    size_t i = 0;
    for (int i4 = 0; i4 < n4; i4++) {
      for (int i3 = 0; i3 < n3; i3++) {
        for (int i2 = 0; i2 < n2; i2++) {
          for (int i1 = 0; i1 < n1; i1++, i++) {
            vec[i] = i1 + i2 * 100 + i3 * 10000 + i4 * 1000000;
          }
        }
      }
    }
  }
  std::vector<int> nb;
  std::vector<int> vec;
};
TEST(toBlock, beg) {
  data4D bigD(20, 20, 20, 20);
  std::vector<int> jw(4, 1), fw(4, 0), nw(4, 10), ng(4, 20);
  std::shared_ptr<dataSubset<int>> smallS(new dataSubset<int>(nw, nw, fw, jw)),
      bigS(new dataSubset<int>(ng, nw, fw, jw));
  block<int> b = block<int>(smallS);

  std::pair<std::shared_ptr<dataSubset<int>>, std::shared_ptr<dataSubset<int>>>
      winds = b.localWindow(ng, nw, fw, jw);
  std::vector<int> smallD(10 * 10 * 10 * 10);

  b.toBlock(bigS, bigD.vec.data(), smallD.data());

  long long i = 0;
  for (int i4 = 0; i4 < 10; i4++) {
    for (int i3 = 0; i3 < 10; i3++) {
      for (int i2 = 0; i2 < 10; i2++) {
        for (int i1 = 0; i1 < 10; i1++, i++) {
          EXPECT_EQ(smallD[i], i1 + i2 * 100 + i3 * 10000 + i4 * 1000000);
        }
      }
    }
  }
}

TEST(toBlock, end) {
  data4D bigD(20, 20, 20, 20);
  std::vector<int> jw(4, 1), fw(4, 10), nw(4, 10), ng(4, 20);
  std::shared_ptr<dataSubset<int>> smallS(new dataSubset<int>(nw, nw, fw, jw)),
      bigS(new dataSubset<int>(ng, nw, fw, jw));
  block<int> b = block<int>(smallS);

  std::pair<std::shared_ptr<dataSubset<int>>, std::shared_ptr<dataSubset<int>>>
      winds = b.localWindow(ng, nw, fw, jw);
  std::vector<int> smallD(10 * 10 * 10 * 10);

  b.toBlock(bigS, bigD.vec.data(), smallD.data());
  long long i = 0;
  for (int i4 = 0; i4 < 10; i4++) {
    for (int i3 = 0; i3 < 10; i3++) {
      for (int i2 = 0; i2 < 10; i2++) {
        for (int i1 = 0; i1 < 10; i1++, i++) {
          EXPECT_EQ(smallD[i], i1 + 10 + (10 + i2) * 100 + (10 + i3) * 10000 +
                                   (10 + i4) * 1000000);
        }
      }
    }
  }
}

TEST(toBlock, jtest) {
  data4D bigD(20, 20, 20, 20);
  std::vector<int> jw(4, 2), fw(4, 0), nw(4, 10), ng(4, 20);
  std::shared_ptr<dataSubset<int>> smallS(new dataSubset<int>(nw, nw, fw, jw)),
      bigS(new dataSubset<int>(ng, nw, fw, jw));
  block<int> b = block<int>(smallS);

  std::pair<std::shared_ptr<dataSubset<int>>, std::shared_ptr<dataSubset<int>>>
      winds = b.localWindow(ng, nw, fw, jw);
  std::vector<int> smallD(10 * 10 * 10 * 10);

  b.toBlock(bigS, bigD.vec.data(), smallD.data());
  long long i = 0;
  for (int i4 = 0; i4 < 10; i4++) {
    for (int i3 = 0; i3 < 10; i3++) {
      for (int i2 = 0; i2 < 10; i2++) {
        for (int i1 = 0; i1 < 10; i1++, i++) {
          EXPECT_EQ(smallD[i], i1 * 2 + (2 * i2) * 100 + (2 * i3) * 10000 +
                                   (2 * i4) * 1000000);
        }
      }
    }
  }
}
