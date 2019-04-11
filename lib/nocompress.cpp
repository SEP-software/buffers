
#include "nocompress.h"
#include <iostream>
#include "SEPException.h"
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

  if (0 == memcpy((void*)x->getPtr(), (const void*)buf->getPtr(), n123)) {
    throw SEPException(std::string("Trouble with memcpy of size ") +
                       std::to_string(n123));
  }
  return x;
}

Json::Value noCompression::getJsonDescription() {
  Json::Value v;
  v["compressType"] = "noCompression";
  v["dataType"] = elementString();
  return v;
}
