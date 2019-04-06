#ifndef ZFP_COMPRESS_H
#define ZFP_COMPRESS_H 1
#include "compress.h"
#include "zfp.h"
#include "zfp/macros.h"

namespace SEP {
namespace IO {

/*!
  Method used to compress dataset
  */
enum zfpMethod {
  ZFP_ACCURACY,   ///< Compress based on maintainin accuracy
  ZFP_RATE,       ///<  Based on constant bitrate for compression
  ZFP_PRECISION,  ///< Based on preserving some precision
  ZFP_UNDEFINED   ///< Undefined method for how to compress data
};

/*!
  Parameters used for ZFP compression
  */
class ZfpParams {
 public:
  //!  Default constructor for ZFP param objects
  ZfpParams() {
    _rate = 7.;
    _tolerance = 3e-1;
    _precision = 15;
    _meth = ZFP_ACCURACY;
  }
  zfpMethod _meth;   ///< Method to use for ZFP compresison
  float _rate;       ///< Rate (bits) o compress data
  int _precision;    ///< Precision to preserve
  float _tolerance;  ///< Tolerance for error
};

/*!
     Compression through ZFP
  */
class ZfpCompression : public compress {
 public:
  //! Create ZFP compression object for JSON description
  /*!
    \param des  JSON description of compression
  */
  ZfpCompression(const Json::Value& des);
  //! Create a ZFP compression object
  /*!
    \param typ Data type (float, int)
    \param pars Parameters to use for comprssion
  */
  ZfpCompression(const SEP::dataType typ, const ZfpParams pars);
  //! Setup ZFP compression class
  void setGlobalZfp();
  //! Compress data
  /*!
    \param n Number of samples in the axis
    \param buf Buffer to compress
  */
  virtual std::shared_ptr<storeBase> compressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  //! Decompress the data
  /*!
    \param n Number of samples in the axis
    \param buf Buffer to decompress
  */
  virtual std::shared_ptr<storeBase> decompressData(
      const std::vector<int> n, const std::shared_ptr<storeBase> buf);
  //! Return JSON description of compression
  virtual Json::Value getJsonDescription();
  //! Return method used to ZFP compression

  std::string methodToString();
  //! Convert string to method
  /*!
    \param meth Method to use compress data
  */
  void stringToMethod(const std::string& meth);

 private:
  zfpMethod _meth;   ///< Method to use for compression
  float _rate;       ///< Rate (bits) o compress data
  float _tolerance;  ///< Tolerance for error
  int _precision;    ///< Precision to preserve
  zfp_type _ztype;   ///< Type of compression
};

}  // namespace IO
}  // namespace SEP
#endif