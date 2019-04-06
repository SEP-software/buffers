#ifndef COMPRESS_H
#define COMPRESS_H 1
#include <json.h>
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
#include "ioTypes.h"
#include "store.h"
namespace SEP {
namespace IO {
/*!
   Virtual class for how to compress a buffer
*/

class compress {
 public:
  //!  Return a compressed version of the buffer
  /*!
    \param n Size of the buffers
    \param buf Buffer to compress
  */
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, std::shared_ptr<storeBase> buf) = 0;
  //!  Return a decompressed version of the buffer
  /*!
    \param n Size of the buffers
    \param buf Buffer to decompress
  */
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, std::shared_ptr<storeBase> buf) = 0;
  //!  Get uncompressed storage object
  /*!
    \param n Size of the buffers
  */
  std::shared_ptr<storeBase> getUncompressedStore(const std::vector<int> n);
  //!  Set the data type for the buffer
  /*!
    \param typ Data type
  */
  void setDataType(const dataType typ) { _typ = typ; }
  //! Return the data type
  dataType getDataType() { return _typ; }
  //! Return the data type as a string
  std::string elementString() { return SEP::getTypeString(_typ); }
  //! Return json description of the compression object
  virtual Json::Value getJsonDescription() = 0;

  dataType _typ;  ///< Data type used by buffer
};

}  // namespace IO
}  // namespace SEP
#endif
