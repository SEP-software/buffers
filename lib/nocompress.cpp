
#include "nocompress.h"
#include <cassert>
using namespace SEP::IO;

std::shared_ptr<storeBase> noCompression::decompressData(
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == IO_BYTE) return buf;
  size_t n123 = getElementSize();
  for (auto n : ns) n123 *= n;
  std::shared_ptr<store<unsigned char>> x(
      new store<unsigned char>(n123 * getElementSize()));
  memcpy(x->getPtr(), buf->getPtr(), n123 * getElementSize());
  return x;
}

std::shared_ptr<storeBase> noCompression::compressData(
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == IO_BYTE) return buf;

  size_t n123 = getElementSize();
  for (auto n : ns) n123 *= n;
  std::shared_ptr<store<unsigned char>> x(new store<unsigned char>(n123));
  memcpy(x->getPtr(), buf->getPtr(), n123 * getElementSize());
  return x;
}

Json::Value noCompression::getJsonDescription() {
  Json::Value v;
  v["compressType"] = "noCompression";
  v["dataType"] = elementString();
  return v;
}
