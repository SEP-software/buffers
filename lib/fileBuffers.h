#ifndef FILE_BUFFERS_H
#define FILE_BUFFERS_H 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffers.h"
#include "compress.h"
#include "fileBuffer.h"
#include "hypercube.h"
#include "memoryUsage.h"
namespace SEP {
namespace IO {
/*!
A class to hold buffers (file) and translates global calls to local calls for
the buffers
*/

class fileBuffers : public buffers {
 public:
  //! Create a file buffers object from parameters
  /*!
    \param hyper Hypercube describeing the data
    \param typ Datatype (float,int, double, etc)
    \param comp Compression object (whether/how data should be compressed)
    \param block Object that determines how to break dataset into parts
    \param mem Oject controlling the amount of memory used
  */
  fileBuffers(const std::shared_ptr<hypercube> hyper, const dataType typ,
              std::shared_ptr<compress> comp = nullptr,
              std::shared_ptr<blocking> block = nullptr,
              std::shared_ptr<memoryUsage> mem = nullptr);
  //! Create a  fileBuffers object from disk
  /*!
    \param hyper Hypercube describeing the data
    \param dir Directory to write the dataset to
    \param jsonArgs JSON object describing how dataset is compressed/split up
    \param mem How to handle memory (when to compress, store to disk)
  */
  fileBuffers(const std::shared_ptr<hypercube> hyper, const std::string dir,
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
};
}  // namespace IO
}  // namespace SEP
#endif
