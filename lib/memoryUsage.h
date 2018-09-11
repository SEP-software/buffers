#ifndef MEMORYUSAGE_H
#define MEMORYUSAGE_H 1
#include <json.h>
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
namespace SEP {
namespace IO {

class memoryReduce {
 public:
  memoryReduce(const std::vector<int> compress, const std::vector<int> toDisk) {
    _compress = compress;
    _toDisk = toDisk;
  }
  std::vector<int> _compress, _toDisk;
};

class memoryUsage {
 public:
  memoryUsage() { _curMem = 0; }

  virtual void updateRecentBuffers(const std::vector<int> &bufs) = 0;
  virtual std::shared_ptr<memoryReduce> changeBufferState(
      const long memChange) = 0;
  virtual void updateMemory(const long memChange) { _curMem += memChange; }

 protected:
  long _curMem = 0;
};

}  // namespace IO
}  // namespace SEP
#endif