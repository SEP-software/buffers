
#include "compress.h"
#include <assert.h>
#include <cmath>
#include <complex>
#include "store.h"
using namespace SEP::IO;

int compress::getElementSize() {
  switch (_typ) {
    case IO_BYTE:
      return 1;
    case IO_INT:
      return 4;
    case IO_FLOAT:
      return 4;
    case IO_COMPLEX:
      return 8;
  }
}
std::shared_ptr<storeBase> compress::getUncompressedStore(
    const std::vector<int> ns) {
  size_t n123 = 1;
  for (auto n : ns) n123 *= n;
  if (_typ == IO_BYTE) {
    std::shared_ptr<store<unsigned char>> x(new store<unsigned char>(n123));
    return x;
  }
  if (_typ == IO_INT) {
    std::shared_ptr<store<int>> x2(new store<int>(n123));
    return x2;
  }
  if (_typ == IO_FLOAT) {
    std::shared_ptr<store<float>> x3(new store<float>(n123));
    return x3;
  }
  if (_typ == IO_COMPLEX) {
    std::shared_ptr<store<std::complex<float>>> x4(
        new store<std::complex<float>>(n123));
    return x4;
  }
  return nullptr;
}
std::string compress::elementString() {
  if (_typ == IO_BYTE) return "BYTE";
  if (_typ == IO_INT) return "INT";
  if (_typ == IO_FLOAT) return "FLOAT";
  if (_typ == IO_DOUBLE) return "DOUBLE";
  if (_typ == IO_COMPLEX) return "COMPLEX";
  return "Unknown";
}
