#ifndef MEMORYUSAGE_H
#define MEMORYUSAGE_H 1
#include <json.h>
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
namespace SEP {
namespace IO {
/*!
  A simple storage class. Contains what buffers should be compressed and what
  should be written to disk.
*/
class memoryReduce {
 public:
  //! Create  memory reduction object
  /*!
    \param compress buffer list that should be compressed
    \param toDisk buffer list that should be stored to disk
  */
  memoryReduce(const std::vector<int> compress, const std::vector<int> toDisk) {
    _compress = compress;
    _toDisk = toDisk;
  }
  std::vector<int> _compress;  ///<  Buffers that should be compressed
  std::vector<int> _toDisk;    ///< List of buffers to write to disk
};
/*!
  Virtual class that controls the memory used by the buffers object. The logic
  is to implement a buffer caching system. Keep track of what buffers have been
  used recently and potentially compress/write to disk.
*/
class memoryUsage {
 public:
  //! Create  virtual class memory usage object

  memoryUsage() { _curMem = 0; }
  //! Update list of buffer recentlty accesed
  /*!
    \param bufs List of recently updated buffers
  */
  virtual void updateRecentBuffers(const std::vector<int> &bufs) = 0;
  //! Return the buffers that should be compressed and written to disk
  /*!
    \param memChange Change in the memory currently used
  */
  virtual std::shared_ptr<memoryReduce> changeBufferState(
      const long memChange) = 0;
  //! Create  memory reduction object
  /*!
    \param memChange Update the amount of memory currently used buffers
  */
  virtual void updateMemory(const long memChange) { _curMem += memChange; }

 protected:
  long _curMem = 0;  ///< Current memory usage
};

}  // namespace IO
}  // namespace SEP
#endif