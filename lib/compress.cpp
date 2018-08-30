
#include "compress.h"
#include <assert.h>
#include <cmath>
#include <complex>
#include "store.h"
using namespace SEP::IO;

std::shared_ptr<storeBase> compress::getUncompressedStore(
    const std::vector<int> ns) {
  size_t n123 = 1;
  for (auto n : ns) n123 *= n;
  if (_typ == DATA_BYTE) {
    std::shared_ptr<storeByte> x(new storeByte(n123));
    return x;
  }
  if (_typ == DATA_INT) {
    std::shared_ptr<storeInt> x2(new storeInt(n123));
    return x2;
  }
  if (_typ == DATA_FLOAT) {
    std::shared_ptr<storeFloat> x3(new storeFloat(n123));
    return x3;
  }
  if (_typ == DATA_COMPLEX) {
    std::shared_ptr<storeComplex> x4(new storeComplex(n123));
    return x4;
  }
  if (_typ == DATA_DOUBLE) {
    std::shared_ptr<storeDouble> x4(new storeDouble(n123));
    return x4;
  }
  return nullptr;
}
