#ifndef COMPRESS_TYPES_H
#define COMPRESS_TYPES_H 1
#include <json.h>
#include <complex>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "SEPException.h"
#include "compress.h"
namespace SEP {
namespace IO {
/*!
  A factory class that  can create all compression objects
*/
class compressTypes {
 public:
  //!  Create compression types from json object
  /*!
    \param val JSON object containing the compression type and the ability to
    create all comoression objets
  */
  compressTypes(const Json::Value &val);
  //!  Return compression object
  std::shared_ptr<compress> getCompressionObj() { return _compress; }
  //!  Return data type for the domain
  dataType getDataType() {
    if (!_compress->_typ) throw SEPException(std::string("Data type not set"));
    return _compress->_typ;
  }

 private:
  std::shared_ptr<compress> _compress = nullptr;  ///< The compression object
};

}  // namespace IO
}  // namespace SEP
#endif