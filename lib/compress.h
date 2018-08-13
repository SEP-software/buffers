#ifndef COMPRESS_H
#define COMPRESS_H 1
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
#include "store.h"
namespace SEP {
namespace IO {

enum compressDataType { IO_BYTE, IO_INT, IO_FLOAT, IO_COMPLEX };

class compress {
 public:
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, std::shared_ptr<storeBase> buf) = 0;
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, std::shared_ptr<storeBase> buf) = 0;
  std::shared_ptr<storeBase> getUncompressedStore(const std::vector<int> n);
  void setCompressDataType(const compressDataType typ) { _typ = typ; }
  int getElementSize();

  compressDataType _typ;
};
class noCompression : public compress {
 public:
  noCompression(const compressDataType typ) { setCompressDataType(typ); }
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
};

}  // namespace IO
}  // namespace SEP
#endif