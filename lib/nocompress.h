#ifndef NO_COMPRESS_H
#define NO_COMPRESS_H 1
#include "compress.h"
namespace SEP {
namespace IO {
/*!
  Default compression object that does no compression
*/
class noCompression : public compress {
 public:
  //! Create no compression object
  /*!
  \param typ Data type(float, byte, etc) */
  noCompression(const dataType typ) { setDataType(typ); }
  //!  Return a storage object after compression. Returns same buffer in this
  //!  case
  /*!
    \param n Size of buffer
    \param buf Storage object
  */
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  //!  Return a storage object after decompression. Returns same buffer in this
  //!  case
  /*!
    \param n Size of buffer
    \param buf Storage object
  */
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  //!  Return JSON description for compression object
  virtual Json::Value getJsonDescription();
};

}  // namespace IO
}  // namespace SEP
#endif