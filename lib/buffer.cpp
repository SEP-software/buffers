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
  _bufferState = UNDEFINED;
  _nameSet = true;
  setBlock();
}
buffer::buffer(const std::vector<int> &n, const std::vector<int> &f,
               std::shared_ptr<compress> comp) {
  _n = n;
  _n123 = 1;
  for (int i : _n) _n123 *= i;
  _f = f;
  _compress = comp;
  _nameSet = false;
  std::shared_ptr<storeByte> bb(new storeByte(0));
  _buf = bb;
  _bufferState = UNDEFINED;

  setBlock();
}
bool checkErrorBitsIn(std::ifstream *f) {
  bool stop = false;

  if (f->eof()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    std::cerr << "1: " << msg << std::endl;
  }

  if (f->fail()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    std::cerr << "2: " << msg << std::endl;
    stop = true;
  }

  if (f->bad()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    std::cerr << "3: " << msg << std::endl;
    stop = true;
  }

  return stop;
}
bool checkErrorBitsOut(std::ofstream *f) {
  bool stop = false;

  if (f->eof()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    std::cerr << "1: " << msg << std::endl;
  }

  if (f->fail()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    std::cerr << "2: " << msg << std::endl;
    stop = true;
  }

  if (f->bad()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    std::cerr << "3: " << msg << std::endl;
    stop = true;
  }

  return stop;
}
long long buffer::readBuffer() {
  long long oldSize = _buf->getSize();

  /*Only need to do something if sitting on disk*/
  if (_bufferState == ON_DISK) {
    assert(_nameSet);
    std::ifstream in(_name, std::iostream::in | std::iostream::binary);

    assert(in);
    in.seekg(0, std::iostream::end);
    int nelemFile = in.tellg();
    in.seekg(0, std::iostream::beg);

    std::shared_ptr<storeByte> x(new storeByte(nelemFile));
    _buf = x;
    in.read(_buf->getPtr(), nelemFile);
    float *xx = (float *)_buf->getPtr();

    assert(!checkErrorBitsIn(&in));

    _bufferState = CPU_COMPRESSED;
    in.close();
  }
  assert(_bufferState != UNDEFINED);
  return _buf->getSize() - oldSize;
}

long long buffer::writeBuffer(bool keepState) {
  long long oldSize = _buf->getSize();

  std::shared_ptr<storeBase> buf;
  bufferState restore;
  assert(_bufferState != UNDEFINED);
  if (_bufferState == ON_DISK) return 0;
  if (keepState) {
    restore = _bufferState;
    buf = _buf->clone();
  }
  _buf->info("before write");

  changeState(CPU_COMPRESSED);
  std::cerr << "COMPRESSS WRITE " << _n123 << "=old new=" << _buf->getSize()
            << " file=" << _name << std::endl;
  assert(_nameSet);
  std::ofstream out(_name, std::ofstream::binary);
  assert(!checkErrorBitsOut(&out));
  out.write(_buf->getPtr(), _buf->getSize());
  assert(!checkErrorBitsOut(&out));

  out.close();

  if (keepState) {
    _buf = buf;
    _bufferState = restore;
  } else {
    _buf->zero();
    _bufferState = ON_DISK;
  }
  std::cerr << "closed file" << std::endl;
  return _buf->getSize() - oldSize;
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
  std::cerr << "in put window cpu " << std::endl;
  bufferState restore = _bufferState;
  std::cerr << "in 2put window cpu " << std::endl;

  long long oldSize = _buf->getSize();
  std::cerr << "in3 put window cpu " << std::endl;

  changeState(CPU_DECOMPRESSED);
  std::cerr << "AXIS 1 " << nwL[0] << "=nw0 fw0=" << fwL[0] << " " << nwG[0]
            << "=ng fg=" << fwG[0] << " blockG=" << blockG[0] << std::endl;
  std::cerr << "AXIS 2 " << nwL[1] << "=nw1 fw1=" << fwL[1] << " " << nwG[1]
            << "=ng fg=" << fwG[1] << " blockG=" << blockG[1] << std::endl;
  std::cerr << "AXIS 3 " << nwL[2] << "=nw2 fw2=" << fwL[2] << " " << nwG[2]
            << "=ng fg=" << fwG[2] << " blockG=" << blockG[2] << std::endl;
  _buf->putWindow(nwL, fwL, jwL, _block, nwG, fwG, blockG, buf);
  std::cerr << "WHAT PUTWINDOW " << std::endl;
  _buf->info("put window");
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
  std::cerr << "CHECK SIZE IN =" << n_w.size() << std::endl;
  size_t i = 0;
  for (i = 0; i < n_w.size(); i++) {
    // Number of samples used before this window
    int nused = ceilf(float(_f[i] - fw[i]) / float(jw[i]));

    std::cerr << "what is the p olem _f=" << _f[i] << " fw=" << f_w[i]
              << " nused=" << nused << std::endl;
    // First sample
    f_w[i] = nused * jw[i] + fw[i] - _f[i];

    // Is the first sample outside this patch?
    assert(f_w[i] < _n[i]);
    // if (f_w[i] > _n[i]) return 0;

    assert(f_w[i] >= 0);
    // subtract off the points already used in previous cells
    n_w[i] = nw[i] - nused;

    // If less 0 we are done
    assert(n_w[i] > 0);
    //    if (n_w[i] < 1) return 0;
    j_w[i] = jw[i];

    // Calculate total number of points in this cell with this sampling

    int npos = ceilf(float(_n[i] - f_w[i]) / float(jw[i]));

    n_w[i] = std::min(n_w[i], npos);
    nelem = nelem * npos;
    fwG[i] = _f[i] - nused;
    assert(fwG[i] >= 0);
    assert(fwG[i] < nw[i]);
    blockG[i + 1] = blockG[i] * nw[i];
    std::cerr << i << " =axis nw=" << nw[i] << " fw=" << fw[i]
              << " n_w=" << n_w[i] << " f_w=" << f_w[i] << " ng=" << nwG[i]
              << " fg=" << fwG[i] << std::endl;
  }
  return nelem;
}

long buffer::changeState(const bufferState state) {
  long long oldSize = _buf->getSize();
  std::cerr << "in change state " << std::endl;
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
          std::cerr << "in undefined" << std::endl;
          _buf = returnStorage(_compress->getDataType(), _n123);
          std::cerr << "return storage" << std::endl;
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
          writeBuffer();
          break;
        default:
          std::cerr << "Unknown conversion" << std::endl;
          assert(1 == 2);
      }
      _bufferState = ON_DISK;
      break;

    default:
      std::cerr << "Can not change state" << std::endl;
      assert(1 == 2);
      break;
  }

  assert(state != UNDEFINED);
  _bufferState = state;
  std::cerr << "SDDFSFDS 1" << std::endl;
  return _buf->getSize() - oldSize;
}
