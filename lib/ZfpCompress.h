#ifndef ZFP_COMPRESS_H
#define ZFP_COMPRESS_H 1
#include "compress.h"
#include "zfp.h"
#include "zfp/macros.h"

namespace SEP {
namespace IO {
enum zfpMethod { ZFP_ACCURACY, ZFP_TOLERANCE, ZFP_PRECISION, ZFP_UNDEFINED };

class ZfpCompression : public compress {
 public:
  ZfpCompression(const Json::Value& des);
  ZfpCompression(const dataType typ, zfpMethod meth, float rate,
                 float tolerance, int precision);
  void setGlobalZfp();
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  virtual std::shared_ptr<storeBase> decompressData(
      const dataType typ, const std::vector<int> n,
      const std::shared_ptr<storeBase> buf);
  virtual Json::Value getJsonDescription();
  std::string methodString();

 private:
  zfpMethod _meth;
  float _rate, _tolerance;
  int _precision;
  zfp_type _ztype;
};

}  // namespace IO
}  // namespace SEP
#endif