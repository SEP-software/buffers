
#include "compressTypes.h"
#include <cassert>
#include <iostream>
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
  }
}

dataType compressTypes::toElementType(const std::string &name) {
  if (name == "BYTE") return IO_BYTE;
  if (name == "INT") return IO_INT;
  if (name == "FLOAT") return IO_FLOAT;
  if (name == "COMPLEX") return IO_COMPLEX;
  return IO_UNKNOWN;
}
