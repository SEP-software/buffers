
#include "nocompress.h"
#include <cassert>
#include <iostream>
using namespace SEP::IO;

std::shared_ptr<storeBase> noCompression::decompressData(
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == DATA_BYTE) return buf;
  size_t n123 = 1;
  for (auto n : ns) n123 *= n;
  std::shared_ptr<storeBase> x = returnStorage(_typ, n123);
  memcpy(x->getPtr(), buf->getPtr(), n123 * getDataTypeSize(_typ));
  return x;
}

std::shared_ptr<storeBase> noCompression::compressData(
    const std::vector<int> ns, const std::shared_ptr<storeBase> buf) {
  if (_typ == DATA_BYTE) return buf;

  size_t n123 = getDataTypeSize(_typ);
  for (auto n : ns) n123 *= n;
  std::shared_ptr<storeByte> x(new storeByte(n123));
  std::cerr << "222 change " << n123 << " " << getDataTypeSize(_typ)
            << std::endl;
  memcpy(x->getPtr(), buf->getPtr(), n123);
  return x;
}

Json::Value noCompression::getJsonDescription() {
  Json::Value v;
  v["compressType"] = "noCompression";
  v["dataType"] = elementString();
  return v;
}
