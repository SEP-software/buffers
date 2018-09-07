
#include <gtest/gtest.h>  // googletest header file
#include <fstream>
#include <iostream>
#include <string>
#include "ZfpCompress.h"
#include "buffer.h"
#include "nocompress.h"
using std::string;
using namespace SEP::IO;
using namespace SEP;

TEST(writeBuffer, buffer) {
  std::shared_ptr<noCompression> comp(new noCompression(DATA_INT));

  std::vector<int> n(3, 20), f(3, 0);

  std::vector<int> a(20 * 20 * 20, 0);
  for (auto i = 0; i < 20 * 20 * 20; i++) a[i] = i;
  std::shared_ptr<storeInt> store(new storeInt(20 * 20 * 20, (void *)a.data()));

  buffer buf(n, f, comp, UNDEFINED);

  std::vector<int> nblock = buf.getBlock();

  EXPECT_EQ(nblock[0], 1);
  EXPECT_EQ(nblock[1], 20);
  EXPECT_EQ(nblock[2], 400);

  buf.setName("/tmp/junk1");

  buf.putBufferCPU(store, CPU_DECOMPRESSED);

  ASSERT_NO_THROW(buf.writeBuffer());
  std::ifstream in("/tmp/junk1", std::ifstream::ate | std::ifstream::binary);

  ASSERT_NO_THROW(in.good());
  size_t nelemFile = in.tellg();
  in.seekg(0, std::iostream::beg);

  EXPECT_EQ(nelemFile, 20 * 20 * 20 * 4);
  std::vector<int> b(8000, 0.);
  in.read((char *)b.data(), nelemFile);

  in.close();
  for (int i = 0; i < 20 * 20 * 20; i += 29) {
    EXPECT_EQ(i, b[i]);
  }
}

std::shared_ptr<storeFloat> array() {
  std::vector<float> ar(8000);
  size_t ii = 0;

  for (auto i3 = 0; i3 < 20; i3++) {
    for (auto i2 = 0; i2 < 20; i2++) {
      for (auto i1 = 0; i1 < 20; i1++, ii++) {
        ar[ii] = (float)rand() / ((float)RAND_MAX) + i1 * .4 + 5.;
      }
    }
  }
  std::shared_ptr<storeFloat> store(new storeFloat(8000, ar.data()));
  return store;
}
TEST(readWindowNoCompress, buffer) {
  ZfpParams zpars = ZfpParams();
  zpars._meth = ZFP_TOLERANCE;
  std::shared_ptr<noCompression> comp(new noCompression(DATA_FLOAT));

  std::shared_ptr<storeFloat> store = array();
  std::vector<int> n(3, 20), f(3, 0);
  buffer buf(n, f, comp, UNDEFINED);
  buf.putBufferCPU(store, CPU_DECOMPRESSED);
  buf.setName("/tmp/junk2");
  std::shared_ptr<storeBase> storeCompare = store->clone();

  ASSERT_NO_THROW(buf.writeBuffer());
  ASSERT_NO_THROW(buf.readBuffer());

  std::shared_ptr<storeFloat> x(new storeFloat(20 * 20 * 20));

  float *b1 = (float *)storeCompare->getPtr();
  buf.getBufferCPU(x, CPU_DECOMPRESSED);
  float *b2 = (float *)x->getPtr();

  for (int ii = 0; ii < 20 * 20 * 20; ii++) {
    ASSERT_LE(fabsf((b1[ii] - b2[ii]) / b2[ii]), .01);
  }
}

TEST(readWindowCompress, buffer) {
  ZfpParams zpars = ZfpParams();
  zpars._meth = ZFP_TOLERANCE;
  std::shared_ptr<ZfpCompression> z(new ZfpCompression(DATA_FLOAT, zpars));

  std::shared_ptr<storeFloat> store = array();
  std::vector<int> n(3, 20), f(3, 0);
  buffer buf(n, f, z, UNDEFINED);
  buf.putBufferCPU(store, CPU_DECOMPRESSED);
  buf.setName("/tmp/junk2");
  std::shared_ptr<storeBase> storeCompare = store->clone();

  ASSERT_NO_THROW(buf.writeBuffer());
  ASSERT_NO_THROW(buf.readBuffer());

  std::shared_ptr<storeFloat> x(new storeFloat(20 * 20 * 20));

  float *b1 = (float *)storeCompare->getPtr();
  buf.getBufferCPU(x, CPU_DECOMPRESSED);
  float *b2 = (float *)x->getPtr();

  for (int ii = 0; ii < 20 * 20 * 20; ii++) {
    ASSERT_LE(fabsf((b1[ii] - b2[ii]) / b2[ii]), .01);
  }
}

/*


  zpars = ZfpParams();
  zpars._meth = ZFP_TOLERANCE;
  compare(zpars, .012, 3., 7.);
  */

TEST(localWindow, buffer) {
  std::shared_ptr<noCompression> comp(new noCompression(DATA_INT));

  std::vector<int> nbuf(7, 20), fbuf(7, 11);

  buffer buf(nbuf, fbuf, comp, UNDEFINED);

  std::vector<int> nw(1, 100), jw(1, 1), fw(1, 0), ng, fg, blockG;
  std::vector<int> nl(1, 1), jl(1, 1), fl(1, 0);

  buf.localWindow(nw, fw, jw, nl, fl, jl, ng, fg, blockG);

  EXPECT_EQ(nl[0], 20);
  EXPECT_EQ(fl[0], 0);

  jw[0] = 3;
  size_t ierr = buf.localWindow(nw, fw, jw, nl, fl, jl, ng, fg, blockG);

  EXPECT_EQ(ierr, 7);
  EXPECT_EQ(nl[0], 7);
  EXPECT_EQ(fl[0], 1);

  fw[0] = 32;
  size_t ierr2 = buf.localWindow(nw, fw, jw, nl, fl, jl, ng, fg, blockG);

  EXPECT_EQ(ierr2, 0);
}

/*
  buffer(const std::string name, const std::vector<int> &n,
         const std::vector<int> &f,
         std::shared_ptr<compress> comp);  // Read from file
  buffer(const std::vector<int> &n, const std::vector<int> &f,
         std::shared_ptr<compress> comp);


  long long getWindowCPU(const std::vector<int> &nw,
                         const std ::vector<int> &fw,
                         const std::vector<int> &jw,
                         std::shared_ptr<storeBase> buf, const size_t bufLoc,
                         const bool keepState = false);

  long long putWindowCPU(const std::vector<int> &nw,
                         const std ::vector<int> &fw, std::vector<int> &jw,
                         const std::shared_ptr<storeBase> buf,
                         const size_t bufLoc, const bool keepState = false);

  long changeState(const bufferState state);
  */