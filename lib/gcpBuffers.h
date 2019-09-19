#ifndef gcp_buffers_h
#define gcp_buffers_h 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffers.h"
#include "compress.h"
#include "gcpBuffer.h"
#include "hypercube.h"
#include "memoryUsage.h"

namespace SEP {
namespace IO {
/*!
A class to hold buffers (GCP objects) and translates global calls to local calls
for the buffers
*/
class gcpBuffers : public buffers {
 public:
  //! Create a GCP buffers object from parameters
  /*!
    \param hyper Hypercube describeing the data
    \param dataType Datatype (float,int, double, etc)
    \param comp Compression object (whether/how data should be compressed)
    \param block Object that determines how to break dataset into parts
    \param mem Oject controlling the amount of memory used
  */
  gcpBuffers(const std::shared_ptr<hypercube> hyper, const dataType dataType,
             std::shared_ptr<blocking> block = nullptr,
             std::shared_ptr<compress> comp = nullptr,
             std::shared_ptr<memoryUsage> mem = nullptr);

  //! Create a  gcpBuffers object from disk
  /*!
    \param hyper Hypercube describeing the data
    \param dir  (bucket/directory) containing dataset
    \param jsonArgs JSON object describing dataset, blocking, compression
    \param mem How to handle memory (when to compress, store to disk)
  */
  gcpBuffers(const std::shared_ptr<hypercube> hyper, const std::string dir,
             const Json::Value &jsonArgs,
             std::shared_ptr<memoryUsage> mem = nullptr);
  //! Set the name for the dataset
  /*!
    \param dir Directory for the dataset
    \param create Whether or not to create buffers
  */
  virtual void setName(const std::string &dir, const bool create) override;
  //! Create all buffers
  /*!
    \param state State for buffers
  */
  virtual void createBuffers(const bufferState state) override;
  //! Gen environment variables
  /*!
    \param key  What we are looking for
    \param defaultV Default value for key
  */

 private:
  std::string _projectID;  ///< GCP project ID
  std::string _region;     ///<  Region where the data is stored
  std::string _bucket;     ///< Name of the GCP bucket
  std::string _baseName;   ///< Base name (directory) for dataset
  int _ntrys;              ///< Number of trys for GCP
};
std::string getEnvVar(std::string const &key, const std::string &defaultV) {
  char *val = getenv(key.c_str());
  return val == NULL ? defaultV : std::string(val);
}
}  // namespace IO
}  // namespace SEP
#endif
