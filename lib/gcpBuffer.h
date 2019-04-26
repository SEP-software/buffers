#ifndef gcp_buffer_h
#define gcp_buffer_h 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "buffer.h"
#include "google/cloud/status_or.h"
#include "google/cloud/storage/client.h"
namespace SEP {
namespace IO {
/*!
  Buffer for Google Cloud Project (GCP).
  Buffers are stored as Object store objects. Datasets are buckets in GCP.
*/
class gcpBuffer : public buffer {
 public:
  //! Create a GCP buffer from an existing object
  /*!
    \param bucketName Bucket and directory for dataset
    \param name Name of object
    \param n Size of domain
    \param f Location of buffer within grid
    \param comp Compression object
  */
  gcpBuffer(const std::string &bucketName, const std::string name,
            const std::vector<int> &n, const std::vector<int> &f,
            std::shared_ptr<compress> comp);

  //! Create a GCP buffer from parameters
  /*!
    \param bucketName Bucket and directory for dataset
    \param n Size of domain
    \param f Location of buffer within grid
    \param comp Compression object
    \param state State for buffet
  */
  gcpBuffer(const std::string &bucketName,

            const std::vector<int> &n, const std::vector<int> &f,
            std::shared_ptr<compress> comp, const bufferState state);

  //! Set the name for the buffer/object
  /*!
    \param name Name of object

  */
  void setBucketName(const std::string name) { _bucketName = name; }
  //! Read the buffer into memory

  virtual long long readBuffer() override;

  //! Write buffer to cloud
  /*!
    \param  keepState Final state for buffer
  */
  virtual long long writeBuffer(bool keepState = false) override;
  virtual ~gcpBuffer() { ; }

 private:
  std::string _bucketName;
};
}  // namespace IO
}  // namespace SEP
#endif
