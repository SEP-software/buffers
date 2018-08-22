#ifndef NO_COMPRESS_H
#define NO_COMPRESS_H 1
#include "compress.h"
namespace SEP {
namespace IO {

class noCompression : public compress {
 public:
  noCompression(const dataType typ) { setDataType(typ); }
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  virtual Json::Value getJsonDescription();
};

}  // namespace IO
}  // namespace SEP
#endif