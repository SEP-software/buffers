#include <assert.h>
#include <buffer.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
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
  _bufferState = ON_DISK;
  _nameSet = true;
  setBlock();
}
buffer::buffer(const int ibuf, const std::vector<int> &n,
               const std::vector<int> &f, std::shared_ptr<compress> comp,
               const bufferState state) {
  _n = n;
  _ibuf = ibuf;
  _n123 = 1;
  for (int i : _n) _n123 *= i;
  _f = f;
  _compress = comp;
  _nameSet = false;
  std::shared_ptr<storeByte> bb(new storeByte(0));
  _buf = bb;
  _bufferState = state;

  setBlock();
}

long long buffer::getBufferCPU(std::shared_ptr<storeBase> buf,
                               const bufferState state) {
  bufferState restore = _bufferState;
  long long oldSize = _buf->getSize();

  std::shared_ptr<storeBase> bufT = _buf;
  changeState(CPU_DECOMPRESSED);
  _buf->getData(buf);
  if (state == restore) {
    _buf = bufT;
    _bufferState = restore;
  } else if (state != CPU_DECOMPRESSED) {
    changeState(state);
  }

  return _buf->getSize() - oldSize;
}

long long buffer::putBufferCPU(std::shared_ptr<storeBase> buf,
                               const bufferState state) {
  bufferState restore = _bufferState;
  long long oldSize = _buf->getSize();
  _buf = _compress->getUncompressedStore(_n);

  _bufferState = CPU_DECOMPRESSED;
  _buf->putData(buf);

  changeState(state);

  return _buf->getSize() - oldSize;
}

long long buffer::getWindowCPU(const std::vector<int> &nwL,
                               const std ::vector<int> &fwL,
                               const std::vector<int> &jwL,
                               const std::vector<int> &nwG,
                               const std ::vector<int> &fwG,
                               const std::vector<int> &blockG, void *buf,
                               const bufferState state) {
  bufferState restore = _bufferState;
  std::shared_ptr<storeBase> bufT = _buf;
  long long oldSize = _buf->getSize();
  changeState(CPU_DECOMPRESSED);
  _buf->getWindow(nwL, fwL, jwL, _block, nwG, fwG, blockG, buf);
  if (restore == state) {
    _bufferState = restore;
    _buf = bufT;
  } else
    changeState(state);
  return _buf->getSize() - oldSize;
}
long long buffer::putWindowCPU(const std::vector<int> &nwL,
                               const std ::vector<int> &fwL,
                               const std::vector<int> &jwL,
                               const std::vector<int> &nwG,
                               const std ::vector<int> &fwG,
                               const std::vector<int> &blockG, const void *buf,
                               const bufferState state) {
  bufferState restore = _bufferState;
  long long oldSize = _buf->getSize();

  changeState(CPU_DECOMPRESSED);

  _buf->putWindow(nwL, fwL, jwL, _block, nwG, fwG, blockG, buf);
  _modified = true;

  changeState(state);

  return _buf->getSize() - oldSize;
}
size_t buffer::localWindow(const std::vector<int> &nw,
                           const std::vector<int> &fw,
                           const std::vector<int> &jw, std::vector<int> &n_w,
                           std::vector<int> &f_w, std::vector<int> &j_w,
                           std::vector<int> &nwG, std::vector<int> &fwG,
                           std::vector<int> &blockG) const {
  size_t nelem = 1;
  blockG.resize(8);
  nwG.resize(7);
  fwG.resize(7);
  blockG[0] = 1;
  size_t i = 0;
  for (i = 0; i < n_w.size(); i++) {
    // Number of samples used before this window
    int nusedLocalBuf = ceilf(float(_f[i] - fw[i]) / float(jw[i]));
    int nusedWindow = ceilf((float)_f[i] / (float)(jw[i]));
    int nusedGlobal = ceilf(float(_f[i]) / float(jw[i]));

    int nbeforeBuffer = 0;
    // First sample
    if (fw[i] > _f[i]) {  // The window starts after the buffer
      f_w[i] = ceilf(float(fw[i] - _f[i]) / jw[i]) * jw[i];
    } else {  // We just need to figure what sample we start at

      f_w[i] =
          ceilf(float(_f[i] - fw[i]) / float(jw[i])) * jw[i] + fw[i] - _f[i];
      nbeforeBuffer = int(float(_f[i] - fw[i]) / float(jw[i]));
    }

    assert(nbeforeBuffer < nw[i]);

    n_w[i] = std::min((int)(ceilf(float(_n[i] - f_w[i]) / float(jw[i]))),
                      nw[i] - nbeforeBuffer);
    // Is the first sample outside this patch?
    assert(f_w[i] < _n[i]);
    // if (f_w[i] > _n[i]) return 0;

    assert(f_w[i] >= 0);
    // subtract off the points already used in previous cells
    // n_w[i] = nw[i] - nusedBuf;

    // If less 0 we are done
    assert(n_w[i] > 0);
    //    if (n_w[i] < 1) return 0;
    j_w[i] = jw[i];

    nelem = nelem * n_w[i];
    if (_f[i] < fw[i]) {
      fwG[i] = 0;
    } else {
      fwG[i] = ceilf(float((_f[i] - fw[i])) / float(jw[i]));
    }
    assert(fwG[i] >= 0);
    assert(fwG[i] < nw[i]);
    blockG[i + 1] = blockG[i] * nw[i];
  }
  return nelem;
}

long buffer::changeState(const bufferState state) {
  long long oldSize = _buf->getSize();
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
        case UNDEFINED:
          _buf = returnStorage(_compress->getDataType(), _n123);
          break;
        default:
          assert(1 == 2);
          break;
      }
      _bufferState = CPU_DECOMPRESSED;
      break;

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
      _bufferState = CPU_COMPRESSED;
      break;

    case ON_DISK:

      switch (_bufferState) {
        case ON_DISK:
          break;
        case CPU_DECOMPRESSED:
          _buf = _compress->compressData(_n, _buf);
          _bufferState = CPU_COMPRESSED;
        case CPU_COMPRESSED:
          if (_modified) {
            writeBuffer();
          }
          break;
        default:
          std::cerr << "Unknown conversion" << std::endl;
          assert(1 == 2);
      }
      _bufferState = ON_DISK;
      _buf->zero();
      break;

    default:
      std::cerr << "Can not change state" << std::endl;
      assert(1 == 2);
      break;
  }

  assert(state != UNDEFINED);
  _bufferState = state;

  return _buf->getSize() - oldSize;
}
