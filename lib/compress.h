#ifndef COMPRESS_H
#define COMPRESS_H 1
#include <json.h>
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
#include "store.h"
namespace SEP {
namespace IO {

class compress {
 public:
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, std::shared_ptr<storeBase> buf) = 0;
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, std::shared_ptr<storeBase> buf) = 0;
  std::shared_ptr<storeBase> getUncompressedStore(const std::vector<int> n);
  void setDataType(const dataType typ) { _typ = typ; }
  dataType getDataType() { return _typ; }
  int getElementSize();
  std::string elementString();
  virtual Json::Value getJsonDescription() = 0;

  dataType _typ;
};

}  // namespace IO
}  // namespace SEP
#endif