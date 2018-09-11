#ifndef SIMPLE_MEMORY_H
#define SIMPLE_MEMORY_H 1
#include <map>
#include "memoryUsage.h"
namespace SEP {
namespace IO {

class simpleMemoryLimit : public memoryUsage {
 public:
  simpleMemoryLimit(const size_t cleanAt);
  virtual void updateRecentBuffers(const std::vector<int> &bufs);
  virtual std::shared_ptr<memoryReduce> changeBufferState(const long memChange);

 private:
  size_t _ibuf = 0, _maxMem;
  std::map<int, size_t> _recent, _status;
  size_t _compressed = -1, _toDisk = -1;
};

}  // namespace IO
}  // namespace SEP
#endif