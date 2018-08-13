#include <assert.h>
#include <buffer.h>
#include <cmath>
#include <fstream>
#include <iostream>
using namespace SEP::IO;

buffer::buffer(const std::string name, const std::vector<int> &n,
               const std::vector<int> &f, std::shared_ptr<compress> comp) {
  _n = n;
  _n123 = 1;
  for (int i : _n) _n123 *= i;
  _f = f;
  _compress = comp;
  _name = name;
  _bufferState = UNDEFINED;
  _nameSet = true;
  setBlock();
}
buffer::buffer(const std::vector<int> &n, const std::vector<int> &f,
               std::shared_ptr<compress> comp) {
  _n = n;
  for (int i : _n) _n123 *= i;
  _f = f;
  _compress = comp;
  _nameSet = false;
  setBlock();
}

void buffer::readBuffer() {
  /*Only need to do something if sitting on disk*/
  if (_bufferState == ON_DISK) {
    assert(_nameSet);
    std::ifstream in(_name, std::ifstream::ate | std::ifstream::binary);
    assert(in.good());
    int nelemFile = in.tellg();
    std::shared_ptr<store<unsigned char>> x(
        new store<unsigned char>(nelemFile));
    std::shared_ptr<storeBase> y = std::dynamic_pointer_cast<storeBase>(x);
    _buf = y;
    in.read(_buf->getPtr(), nelemFile);
    assert(in);
    _bufferState = CPU_COMPRESSED;
    in.close();
  }
  assert(_bufferState != UNDEFINED);
}

void buffer::writeBuffer(bool keepState) {
  std::shared_ptr<storeBase> buf;
  bufferState restore;
  assert(_bufferState != UNDEFINED);
  if (_bufferState == ON_DISK) return;
  if (keepState) {
    restore = _bufferState;
    buf = _buf->clone();
  }
  changeState(CPU_COMPRESSED);
  assert(_nameSet);
  std::ofstream out(_name, std::ofstream::binary);
  assert(out.good());
  out.write(_buf->getPtr(), _buf->getSize());
  assert(out.good());
  out.close();
  if (keepState) {
    _buf = buf;
    _bufferState = restore;
  }
}
void buffer::getBufferCPU(std::shared_ptr<storeBase> buf, bool keepState) {
  bufferState restore = _bufferState;
  std::shared_ptr<storeBase> bufT = _buf;

  changeState(CPU_DECOMPRESSED);
  _buf->getData(buf);
  if (keepState) {
    _buf = bufT;
    _bufferState = restore;
  }
}

void buffer::putBufferCPU(std::shared_ptr<storeBase> buf, bool keepState) {
  bufferState restore = _bufferState;
  _buf = _compress->getUncompressedStore(_n);
  _buf->putData(buf);
  if (keepState) changeState(restore);
}

void buffer::getWindowCPU(const std::vector<int> &nw,
                          const std ::vector<int> &fw,
                          const std::vector<int> &jw,
                          std::shared_ptr<storeBase> buf, const size_t loc,
                          const bool keepState) {
  bufferState restore = _bufferState;
  std::shared_ptr<storeBase> bufT = _buf;

  changeState(CPU_DECOMPRESSED);
  std::shared_ptr<storeBase> bufIn = _buf->clone();
  _buf->getWindow(nw, fw, jw, _block, bufIn, loc);
  if (keepState) changeState(restore);
  _bufferState = restore;
  _buf = bufT;
}
void buffer::putWindowCPU(const std::vector<int> &nw,
                          const std ::vector<int> &fw, std::vector<int> &jw,
                          const std::shared_ptr<storeBase> buf,
                          const size_t loc, const bool keepState) {
  bufferState restore = _bufferState;

  changeState(CPU_DECOMPRESSED);
  _buf->putWindow(nw, fw, jw, _block, buf, loc);
  if (keepState) changeState(restore);
}
size_t buffer::localWindow(const std::vector<int> &nw,
                           const std::vector<int> &fw,
                           const std::vector<int> &jw, std::vector<int> &n_w,
                           std::vector<int> &f_w, std::vector<int> &j_w) const {
  size_t nelem = 1;
  for (auto i = 0; i < n_w.size(); i++) {
    int nuse = ceilf(float(_f[i] - f_w[i]) / float(jw[i]));
    f_w[i] = nuse + fw[i] - _f[i];
    if (f_w[i] > _n[i]) return 0;
    n_w[i] = nw[i] - nuse;
    if (n_w[i] < 1) return 0;
    j_w[i] = jw[i];
    int npos = ceilf(float(_n[i] - f_w[i]) / float(jw[i]));
    n_w[i] = std::min(n_w[i], npos);
    nelem = nelem * npos;
  }
  return nelem;
}

void buffer::changeState(const bufferState state) {
  switch (state) {
    case CPU_DECOMPRESSED:
      switch (_bufferState) {
        case ON_DISK:
          readBuffer();
        case CPU_COMPRESSED:
          _buf = _compress->decompressData(_n, _buf);
          break;
        case CPU_DECOMPRESSED:
          break;
        default:
          std::cerr << "Unknown conversion" << std::endl;
          assert(1 == 2);
      }
    case CPU_COMPRESSED:
      switch (_bufferState) {
        case ON_DISK:
          readBuffer();
          break;
        case CPU_DECOMPRESSED:
          _buf = _compress->compressData(_n, _buf);
          break;
        case CPU_COMPRESSED:
          break;
        default:
          std::cerr << "Unknown conversion" << std::endl;
          assert(1 == 2);
      }
    case ON_DISK:
      switch (_bufferState) {
        case ON_DISK:
          break;
        case CPU_DECOMPRESSED:
          _buf = _compress->compressData(_n, _buf);
          writeBuffer();
          break;
        case CPU_COMPRESSED:
          writeBuffer();
          break;
        default:
          std::cerr << "Unknown conversion" << std::endl;
          assert(1 == 2);
      }
    default:
      std::cerr << "Can not change state" << std::endl;
      assert(1 == 2);
  }
  assert(state != UNDEFINED);
  _bufferState = state;
}
