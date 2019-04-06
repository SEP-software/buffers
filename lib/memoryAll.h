#ifndef MEMORY_ALL_H
#define MEMORY_ALL_H 1
#include "memoryUsage.h"
namespace SEP {
namespace IO {
/*!
  Default memory class that does nothing (e.g. does not constrian memory usage
  in anyway). Assumes unlimited memory.
*/
class memoryAll : public memoryUsage {
 public:
  //! Create memory management object
  memoryAll() { ; }
  //! Implementation of virtual class that keeps track of what buffers have been
  //! used recently.
  /*!
    \param bufs Buffer numbers that have been most recently accessed
   */
  virtual void updateRecentBuffers(const std::vector<int> &bufs);
  //! Returns how buffers should be changed given the last operation on buffers
  //! object. Does nothing in this class.
  /*!
    \param memChange How the amount of memory used has changed by last operation
    on bufferts object
    */
  virtual std::shared_ptr<memoryReduce> changeBufferState(const long memChange);
};

}  // namespace IO
}  // namespace SEP
#endif