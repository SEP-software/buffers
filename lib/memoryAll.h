#ifndef MEMORY_ALL_H
#define MEMORY_ALL_H 1
#include "memoryUsage.h"
namespace SEP {
namespace IO {

class memoryAll : public memoryUsage {
 public:
  memoryAll() { ; }
  virtual void updateRecentBuffers(const std::vector<int> &bufs);
  virtual std::shared_ptr<memoryReduce> changeBufferState(const long memChange);
};

}  // namespace IO
}  // namespace SEP
#endif