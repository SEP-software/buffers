
#include "compressTypes.h"
#include "buffersConfig.h"
#include <iostream>
#ifdef USE_ZFP
#include "ZfpCompress.h"
#endif
#include "SEPException.h"
#include "nocompress.h"
using namespace SEP::IO;

std::string zfp="ZfpCompression";
compressTypes::compressTypes(const Json::Value &des) {
  if (des["dataType"].isNull())
    throw SEPException(std::string("dataType not in parameters"));
  if (des["compressType"].isNull())
    throw SEPException(std::string("compressTyp not in parameters"));
  std::string typ = des["compressType"].asString();
  dataType ele = toElementType(des["dataType"].asString());
  if (typ == std::string("noCompression")) {
    _compress.reset(new noCompression(ele));
#ifdef USE_ZFP
  } else if (typ == zfp) {
    _compress.reset(new ZfpCompression(des));
#endif
  } else {
    throw SEPException(std::string("Unknown compression type " + typ+" "+zfp));
  }
}
