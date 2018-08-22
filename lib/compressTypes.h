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
  dataType getDataType() { return _compress->_typ; }
  dataType toElementType(const std::string &name);

 private:
  std::shared_ptr<compress> _compress;
};

}  // namespace IO
}  // namespace SEP
#endif