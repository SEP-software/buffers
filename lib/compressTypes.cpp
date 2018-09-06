
#include "compressTypes.h"
#include <cassert>
#include <iostream>
#include "ZfpCompress.h"
#include "nocompress.h"
using namespace SEP::IO;

compressTypes::compressTypes(const Json::Value &des) {
  if (des["dataType"].isNull()) {
    std::cerr << std::string(
                     "trouble grabbing parameter compressType from parameters")
              << std::endl;

    assert(1 == 2);
  }
  if (des["compressType"].isNull()) {
    std::cerr << std::string(
                     "trouble grabbing parameter compressType from parameters")
              << std::endl;

    assert(1 == 2);
  }
  std::string typ = des["compressType"].asString();
  dataType ele = toElementType(des["dataType"].asString());
  if (typ == std::string("noCompression")) {
    _compress.reset(new noCompression(ele));
  } else if (typ == std::string("ZfpCompression")) {
    _compress.reset(new ZfpCompression(des));
  }
}
