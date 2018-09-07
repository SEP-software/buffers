#ifndef BUFFERS_H
#define BUFFERS_H 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffer.h"
#include "compress.h"
#include "hypercube.h"
#include "memoryUsage.h"
namespace SEP {
namespace IO {
class buffers {
 public:
  buffers(const std::shared_ptr<hypercube>, const dataType dataType,
          std::shared_ptr<compress> comp = nullptr,
          std::shared_ptr<blocking> block = nullptr,
          std::shared_ptr<memoryUsage> mem = nullptr);
  buffers(const std::shared_ptr<hypercube> hyper, const std::string dir,
          const Json::Value &jsonArgs,
          std::shared_ptr<memoryUsage> mem = nullptr);

  void getWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw, void *buf);
  void putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw, const void *buf);
  void createBuffers(const bufferState state);
  void changeState(const bufferState state);
  Json::Value getDescription();
  void updateMemory(const long change);
  void setMemoryUsage(std::shared_ptr<memoryUsage> mem) { _memory = mem; }
  std::shared_ptr<compress> createDefaultCompress();
  std::shared_ptr<blocking> createDefaultBlocking();
  std::shared_ptr<memoryUsage> createDefaultMemory();
  void setDefaultState(const bufferState stat) {
    _defState = stat;
    _defaultStateSet = true;
  }
  void setDirectory(const std::string &dir, const bool createDirectory);
  std::vector<int> parsedWindows(const std::vector<int> &nw,
                                 const std ::vector<int> &fw,
                                 const std::vector<int> &jw);
  std::shared_ptr<storeBase> getSpecificStore(int ibuf) {
    return _buffers[ibuf].getStorePtr();
  }

 private:
  bool _defaultStateSet;
  bufferState _defState;
  std::shared_ptr<blocking> _blocking;
  std::shared_ptr<memoryUsage> _memory;
  std::shared_ptr<compress> _compress;
  dataType _typ;
  std::shared_ptr<hypercube> _hyper;
  std::vector<buffer> _buffers;
  std::vector<int> _lastUsed;
  std::vector<std::vector<int>> _axisBlocking;
  std::vector<int> _n123blocking;
  std::string _directory;
};
}  // namespace IO
}  // namespace SEP
#endif
