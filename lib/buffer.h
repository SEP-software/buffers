#ifndef buffer_h
#define buffer_h 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "SEPException.h"
#include "compress.h"
namespace SEP {
namespace IO {
//! Different status for buffers

enum bufferState {
  UNDEFINED,         /// Undefined status
  CPU_COMPRESSED,    /// Buffer is compressed
  CPU_DECOMPRESSED,  /// Buffer is decompressed
  ON_DISK            /// Buffer is on disk
};

//! Convert buffer stat to a readable string
std::string bufferStateToString(const bufferState &state);

/*!
  Virtual buffer object
*/

class buffer {
 public:
  buffer() { ; }
  void createStorageBuffer() {
    std::shared_ptr<storeByte> bb(new storeByte(0));
    _buf = bb;
  }
  void setBufferState(const bufferState state) { _bufferState = state; }
  void setCompress(std::shared_ptr<compress> comp) { _compress = comp; }
  void setLoc(const std::vector<int> &n, const std::vector<int> &f) {
    _f = f;
    _n = n;
    _n123 = 1;
    for (int i : _n) _n123 *= i;
    setBlock();
  }

  void setName(const std::string name) {
    _name = name;
    _nameSet = true;
  }
  std::string getFileName() {
    if (!_nameSet) throw SEPException(std::string("Name of file not set"));
    size_t found = _name.find_last_of("/\\");

    return _name.substr(found + 1);
  }
  void setBlock() {
    _block.push_back(1);
    for (size_t i = 0; i < _n.size(); i++) _block.push_back(_block[i] * _n[i]);
  }
  virtual long long readBuffer() = 0;
  virtual long long writeBuffer(bool keepState = false) = 0;
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
  virtual ~buffer() { ; }

  std::vector<int> _f;
  std::vector<int> _n;
  std::vector<int> _block;

 protected:
  std::shared_ptr<storeBase> _buf;
  std::shared_ptr<compress> _compress;
  std::string _name;
  bool _nameSet;
  bool _modified = false;
  int _ibuf;
  long long _n123;
  bufferState _bufferState = UNDEFINED;
};
}  // namespace IO
}  // namespace SEP
#endif
