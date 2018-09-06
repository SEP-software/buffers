#ifndef ZFP_COMPRESS_H
#define ZFP_COMPRESS_H 1
#include "compress.h"
#include "zfp.h"
#include "zfp/macros.h"

namespace SEP {
namespace IO {

enum zfpMethod { ZFP_ACCURACY, ZFP_TOLERANCE, ZFP_PRECISION, ZFP_UNDEFINED };

class ZfpParams {
 public:
  ZfpParams() {
    _rate = 7.;
    _tolerance = 3e-1;
    _precision = 15;
    _meth = ZFP_ACCURACY;
  }
  zfpMethod _meth;
  float _rate;
  int _precision;
  float _tolerance;
};

class ZfpCompression : public compress {
 public:
  ZfpCompression(const Json::Value& des);
  ZfpCompression(const dataType typ, const ZfpParams pars);

  void setGlobalZfp();
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  virtual Json::Value getJsonDescription();
  std::string methodToString();
  void stringToMethod(const std::string meth);

 private:
  zfpMethod _meth;
  float _rate, _tolerance;
  int _precision;
  zfp_type _ztype;
};

}  // namespace IO
}  // namespace SEP
#endif