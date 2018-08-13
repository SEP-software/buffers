#ifndef BUFFER_H
#define BUFFER_H 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "compress.h"
namespace SEP {
namespace IO {
enum bufferState { UNDEFINED, CPU_COMPRESSED, CPU_DECOMPRESSED, ON_DISK };
class buffer {
 public:
  buffer(const std::string name, const std::vector<int> &n,
         const std::vector<int> &f,
         std::shared_ptr<compress> comp);  // Read from file
  buffer(const std::vector<int> &n, const std::vector<int> &f,
         std::shared_ptr<compress> comp);
  void setName(const std::string name) { _name = name; }
  void setBlock() {
    _block.push_back(1);
    for (size_t i = 0; i < _n.size(); i++) _block.push_back(_block[i] * _n[i]);
  }
  virtual void readBuffer();
  virtual void writeBuffer(bool keepState = false);
  virtual void getBufferCPU(std::shared_ptr<storeBase> buf,
                            bool keepState = false);
  virtual void putBufferCPU(std::shared_ptr<storeBase> buf,
                            bool keepState = false);
  void getWindowCPU(const std::vector<int> &nw, const std ::vector<int> &fw,
                    const std::vector<int> &jw, std::shared_ptr<storeBase> buf,
                    const size_t bufLoc, const bool keepState = false);

  void putWindowCPU(const std::vector<int> &nw, const std ::vector<int> &fw,
                    std::vector<int> &jw, const std::shared_ptr<storeBase> buf,
                    const size_t bufLoc, const bool keepState = false);
  size_t localWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                     const std::vector<int> &jw, std::vector<int> &n_w,
                     std::vector<int> &f_w, std::vector<int> &j_w) const;
  void changeState(const bufferState state);

 private:
  std::vector<int> _f, _n, _block;
  std::shared_ptr<storeBase> _buf;
  std::shared_ptr<compress> _compress;
  std::string _name;
  bool _nameSet;
  long long _n123;
  bufferState _bufferState;
};
}  // namespace IO
}  // namespace SEP
#endif
