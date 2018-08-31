
#include <gtest/gtest.h>  // googletest header file
#include <fstream>
#include <iostream>
#include <string>
#include "ZfpCompress.h"
#include "buffer.h"
#include "buffers.h"
#include "nocompress.h"
using std::string;
using namespace SEP::IO;
using namespace SEP;

TEST(parsedWindows, buffers) {
  std::vector<SEP::axis> axes;
  axes.push_back(SEP::axis(100, 0., .004, "time"));
  axes.push_back(SEP::axis(100, 0., .2, "offset"));
  axes.push_back(SEP::axis(100, 0., .2, "midpoint"));
  std::shared_ptr<SEP::hypercube> hyper(new SEP::hypercube(axes));

  std::vector<int> blocksize(3, 5), nb(3, 10);
  std::shared_ptr<blocking> block(new blocking(blocksize, nb));
  std::shared_ptr<noCompression> comp(new noCompression(SEP::DATA_INT));

  ASSERT_NO_THROW(buffers myb(hyper, SEP::DATA_INT, comp, block));

  buffers myb(hyper, SEP::DATA_INT, comp, block);
  std::vector<int> nw(3, 1), fw(3, 0), jw(3, 1);

  ASSERT_NO_THROW(std::vector<int> windows = myb.parsedWindows(nw, fw, jw));

  std::vector<int> windows = myb.parsedWindows(nw, fw, jw);
  EXPECT_EQ((int)windows.size(), 1);
  EXPECT_EQ(windows[0], 0);

  nw[0] = 11;

  windows = myb.parsedWindows(nw, fw, jw);
  EXPECT_EQ((int)windows.size(), 2);
  EXPECT_EQ(windows[0], 0);
  EXPECT_EQ(windows[1], 1);

  nw[0] = 4;
  jw[0] = 14;
  fw[0] = 10;
  windows = myb.parsedWindows(nw, fw, jw);
  EXPECT_EQ((int)windows.size(), 4);
  EXPECT_EQ(windows[0], 1);
  EXPECT_EQ(windows[1], 2);
  EXPECT_EQ(windows[2], 3);
  EXPECT_EQ(windows[3], 5);
}

TEST(bigTest, buffers) {
  std::vector<SEP::axis> axes;

  axes.push_back(SEP::axis(100, 0., .004, "time"));
  axes.push_back(SEP::axis(100, 0., .2, "offset"));
  axes.push_back(SEP::axis(100, 0., .2, "midpoint"));
  std::shared_ptr<SEP::hypercube> hyper(new SEP::hypercube(axes));

  std::vector<int> blocksize(3, 5), nb(3, 10);
  std::shared_ptr<blocking> block(new blocking(blocksize, nb));
  std::shared_ptr<noCompression> comp(new noCompression(SEP::DATA_FLOAT));
  buffers myb(hyper, SEP::DATA_FLOAT, comp, block);

  std::shared_ptr<storeFloat> whole(new storeFloat(100 * 100 * 100)),
      result(new storeFloat(100 * 100 * 100));

  float *rawP = (float *)whole->getPtr(), *rawC = (float *)result->getPtr();

  size_t i = 0;
  for (int i3 = 0; i3 < 100; i3++) {
    for (int i2 = 0; i2 < 100; i2++) {
      for (int i1 = 0; i1 < 100; i1++) {
        rawP[i++] = i1 + i2 * 1000 + i3 * 1000 * 1000;
      }
    }
  }

  std::vector<int> ns(3, 100), fs(3, 0), js(3, 1);
  myb.putWindow(ns, fs, js, whole);
  std::cerr << "do ne with put window " << std::endl;

  /*std::shared_ptr<storeBase> bs = myb.getSpecificStore(1);
  std::shared_ptr<storeFloat> in = std::dynamic_pointer_cast<storeFloat>(bs);
  float *inP = (float *)in->getPtr();
  for (size_t ii = 0; ii < 10 * 10 * 10; ii++) {
    std::cerr << ii << " BLOCK2 " << inP[ii] << std::endl;
  }

  rawC = (float *)result->getPtr();
  for (size_t ii = 0; ii < 100 * 100 * 100; ii++) {
    std::cerr << ii << " " << rawP[ii] << "=in out=" << rawC[ii] << std::endl;
  }
  */
  myb.getWindow(ns, fs, js, result);
  std::cerr << "done with get window" << std::endl;
  for (size_t ii = 0; ii < 100 * 100 * 100; ii += 23) {
    EXPECT_EQ(rawP[ii], rawC[ii]);
  }
}

/*
  buffers(const std::shared_ptr<hypercube> hyper, const Json::Value &jsonArgs,
          std::shared_ptr<memoryUsage> mem = nullptr);

  void getWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw, std::shared_ptr<storeBase> buf,
                 const bool keepState = false);
  void putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw,
                 const std::shared_ptr<storeBase> buf,
                 const bool keepState = false);
  void createBuffers();
  Json::Value getDescription();
  void updateMemory(const long change);
  std::shared_ptr<compress> createDefaultCompress();
  std::shared_ptr<blocking> createDefaultBlocking();
  std::shared_ptr<memoryUsage> createDefaultMemory();
  Json::Value getFiles();
  void setDirectory(std::string &dir);
  std::vector<int> parsedWindows(const std::vector<int> &nw,
                                 const std ::vector<int> &fw,
                                 const std::vector<int> &jw);

                                 */
