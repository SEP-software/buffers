#ifndef COMPRESS_TYPES_H
#define COMPRESS_TYPES_H 1
#include <json.h>
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
#include "compress.h"
namespace SEP {
namespace IO {

class compressTypes {
 public:
  compressTypes(const Json::Value &val);
  std::shared_ptr<compress> getCompressionObj() { return _compress; }
  dataType getDataType() {
    assert(_compress->_typ);
    return _compress->_typ;
  }

 private:
  std::shared_ptr<compress> _compress = nullptr;
};

}  // namespace IO
}  // namespace SEP
#endif