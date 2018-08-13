#ifndef BUFFERS_H
#define BUFFERS_H 1
#include <store.h>
#include <cstring>
#include <hypercube>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffer.h"
#include "compress.h"
namespace SEP {
namespace IO {
class buffers {
 public:
  buffers(std::shared_ptr<hypercube>, std::shared_ptr<compress> comp = nullptr,
          std::shared_ptr<blocking> block = nullptr);
  buffers(std::string diretory, std::shared_ptr<compress> comp = nullptr,
          std::shared_ptr<blocking> block = nullptr);

  void readDescription();
  void writeDescription();
  void readBlocking();
  void writeBlocking();
  void getWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw, std::shared_ptr<storeBase> buf,
                 const bool keepState = false);
  void putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 std::vector<int> &jw, const std::shared_ptr<storeBase> buf,
                 const bool keepState = false);
  void createBuffers();
  std::shared_ptr<compress> createDefaultCompress()

      std::shared_ptr<blocking> createDefaultBlocking();
  void setDirectory(std::string dir) { _directory = dir; }
  std::vector<int> parsedWindows(const std::vector<int> &nw,
                                 const std ::vector<int> &fw,
                                 const std::vector<int> &jw);

 private:
  std::shared_ptr<blocking> _blocking;
  std::shared_ptr<compress> _compress;
  std::shared_ptr<hypercube> _hyp;
  std::vector<buffer> _buffers;
  std::vector<int> _lastUsed;
  std::vector<std::vector<int>> _axisBlocking;
  std::vector<int> _nblocking;
  std::string _directory;
};
}  // namespace IO
}  // namespace SEP
#endif
