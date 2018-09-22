#pragma once
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
  buffer(const int ibuf, const std::vector<int> &n, const std::vector<int> &f,
         std::shared_ptr<compress> comp, const bufferState state);
  void setName(const std::string name) {
    _name = name;
    _nameSet = true;
  }
  std::string getFileName() {
    assert(_nameSet);
    size_t found = _name.find_last_of("/\\");

    return _name.substr(found + 1);
  }
  void setBlock() {
    _block.push_back(1);
    for (size_t i = 0; i < _n.size(); i++) _block.push_back(_block[i] * _n[i]);
  }
  virtual long long readBuffer();
  virtual long long writeBuffer(bool keepState = false);
  virtual long long getBufferCPU(std::shared_ptr<storeBase> buf,
                                 const bufferState finalState);
  virtual long long putBufferCPU(std::shared_ptr<storeBase> buf,
                                 const bufferState finalState);
  long long getWindowCPU(const std::vector<int> &nwL,
                         const std ::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nwG,
                         const std ::vector<int> &fwG,
                         const std::vector<int> &blockG, void *buf,
                         const bufferState finalState);

  long long putWindowCPU(const std::vector<int> &nwL,
                         const std ::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nwG,
                         const std ::vector<int> &fwG,
                         const std::vector<int> &blockG, const void *buf,
                         const bufferState finalState);
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
  bool _nameSet, _modified = false;
  int _ibuf;
  long long _n123;
  bufferState _bufferState;
};
}  // namespace IO
}  // namespace SEP
#endif
