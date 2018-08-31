#include <gtest/gtest.h>  // googletest header file
#include <fstream>
#include <iostream>
#include <string>
#include "ZfpCompress.h"
#include "ioTypes.h"
using std::string;
using namespace SEP::IO;

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

void compare(const ZfpParams zpars, const float maxE, const float minCompress,
             const float maxCompress) {
  ZfpCompression z = ZfpCompression(SEP::DATA_FLOAT, zpars);
  std::vector<int> n(3, 20);
  std::shared_ptr<storeFloat> st = array();

  float *ar2 = (float *)st->getPtr();
  std::shared_ptr<storeBase> compressData = z.compressData(n, st);

  std::shared_ptr<storeBase> decompressData = z.decompressData(n, compressData);
  float err = 0;
  float *comp = (float *)decompressData->getPtr();
  for (auto ii = 0; ii < 20 * 20 * 20; ii++) {
    ASSERT_LE(fabsf((comp[ii] - ar2[ii]) / ar2[ii]), maxE);
    err = std::max(err, (comp[ii] - ar2[ii]) / ar2[ii]);
  }

  float ratio = float(20 * 20. * 20. * 4.) / ((float)compressData->getSize());

  ASSERT_LE(ratio, maxCompress);
  ASSERT_GE(ratio, minCompress);

  std::cerr << "RATIO ="
            << float(20 * 20. * 20. * 4.) / ((float)compressData->getSize())
            << " MaxError=" << err << std::endl;
}

TEST(testAccuracyFloat, zfpCompress) {
  ZfpParams zpars = ZfpParams();
  compare(zpars, .012, 3., 7.);

  zpars = ZfpParams();
  zpars._meth = ZFP_TOLERANCE;
  compare(zpars, .012, 3., 7.);

  zpars = ZfpParams();
  zpars._meth = ZFP_PRECISION;
  compare(zpars, .012, 3., 7.);
}
