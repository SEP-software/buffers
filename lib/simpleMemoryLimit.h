#ifndef SIMPLE_MEMORY_H
#define SIMPLE_MEMORY_H 1
#include <map>
#include "memoryUsage.h"
namespace SEP {
namespace IO {
/*!
   Simple memory limiting memory usage class. Compressses and then sends to disk
   blocks in order to get below memory limit specified at initialization
*/
class simpleMemoryLimit : public memoryUsage {
 public:
  //! Initialize simple memory object with max-memory to use
  /*!
    \param cleanAt
    */
  simpleMemoryLimit(const size_t cleanAt);
  //! Update list of buffer recentlty accesed
  /*!
    \param bufs List of recently updated buffers
  */
  virtual void updateRecentBuffers(const std::vector<int> &bufs);
  //! Return the buffers that should be compressed and written to disk
  /*!
    \param memChange Change in the memory currently used
  */
  virtual std::shared_ptr<memoryReduce> changeBufferState(const long memChange);

 private:
  size_t _ibuf = 0;               ///< Counter for most recent activities
  size_t _maxMem;                 ///<  Maximum memory to ue for buffers
  std::map<int, size_t> _recent;  ///< How recently a buffer has been touched
  std::map<int, int> _status;     ///< Status (location) of buffer
  int _compressed = -1;           ///< Status is compressed
  int _toDisk = -1;               ///< Status is on disk
};

}  // namespace IO
}  // namespace SEP
#endif