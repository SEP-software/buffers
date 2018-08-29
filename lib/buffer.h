#ifndef BUFFER_H
#define BUFFER_H 1
#include <store.h>
#include <cassert>
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
  void setName(const std::string name) {
    _name = name;
    _nameSet = true;
  }
  std::string getName() {
    assert(_nameSet);
    return _name;
  }
  void setBlock() {
    _block.push_back(1);
    for (size_t i = 0; i < _n.size(); i++) _block.push_back(_block[i] * _n[i]);
  }
  virtual long long readBuffer();
  virtual long long writeBuffer(bool keepState = false);
  virtual long long getBufferCPU(std::shared_ptr<storeBase> buf,
                                 bool keepState = false);
  virtual long long putBufferCPU(std::shared_ptr<storeBase> buf,
                                 bool keepState = false);
  long long getWindowCPU(
      const std::vector<int> &nwL, const std ::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nwG,
      const std ::vector<int> &fwG, const std::vector<int> &blockG,
      std::shared_ptr<storeBase> buf, const bool keepState = false);

  long long putWindowCPU(
      const std::vector<int> &nwL, const std ::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nwG,
      const std ::vector<int> &fwG, const std::vector<int> &blockG,
      const std::shared_ptr<storeBase> buf, const bool keepState = false);
  size_t localWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                     const std::vector<int> &jw, std::vector<int> &n_w,
                     std::vector<int> &f_w, std::vector<int> &j_w,
                     std::vector<int> &nwG, std::vector<int> &fwG,
                     std::vector<int> &blockG) const;
  long changeState(const bufferState state);
  std::vector<int> getBlock() { return _block; }
  std::shared_ptr<storeBase> getStorePtr() { return _buf; }

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
