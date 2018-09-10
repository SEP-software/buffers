#include "buffers.h"
#include <sys/stat.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

#include <tbb/tbb.h>
#include "compressTypes.h"
#include "memoryAll.h"
#include "nocompress.h"
using namespace SEP::IO;

std::shared_ptr<blocking> buffers::createDefaultBlocking() {
  std::vector<int> bs(3, 1), block(3, 1);
  if (_hyper->getAxis(1).n > 16)
    bs[0] = 1;
  else
    bs[0] = 16;

  if (_hyper->getNdimG1() == 1) {
    block[0] = 256 * 1024;
  } else if (_hyper->getNdimG1() == 2) {
    bs[1] = std::min(_hyper->getAxis(2).n, 4);
    block[0] = 256;
    block[1] = 1024;
  } else {
    bs[1] = std::min(_hyper->getAxis(2).n, 4);
    bs[2] = std::min(_hyper->getAxis(3).n, 4);
    block[0] = 256;
    block[1] = 32;
    block[2] = 32;
  }
  std::shared_ptr<blocking> b(new blocking(bs, block));
  return b;
}
std::shared_ptr<compress> buffers::createDefaultCompress() {
  std::shared_ptr<compress> c(new noCompression(_typ));
  return c;
}
std::shared_ptr<memoryUsage> buffers::createDefaultMemory() {
  std::shared_ptr<memoryUsage> c(new memoryAll());
  return c;
}
buffers::buffers(const std::shared_ptr<hypercube> hyper, const std::string dir,
                 const Json::Value &des, std::shared_ptr<memoryUsage> mem) {
  _hyper = hyper->clone();
  if (des["blocking"].isNull()) {
    std::cerr << std::string(
                     "trouble grabbing parameter blocking from parameters")
              << std::endl;

    assert(1 == 2);
  }

  _blocking.reset(new blocking(des["blocking"]));

  if (des["compression"].isNull()) {
    std::cerr << std::string(
                     "trouble grabbing parameter blocking from parameters")
              << std::endl;

    assert(1 == 2);

    _memory = mem;
    if (!_memory) {
      _memory = createDefaultMemory();
    }
  }

  SEP::IO::compressTypes ct = compressTypes(des["compression"]);

  _compress = ct.getCompressionObj();

  _defaultStateSet = false;
  createBuffers(ON_DISK);
  setDirectory(dir, false);
}
Json::Value buffers::getDescription() {
  Json::Value des;

  des["blocking"] = _blocking->getJsonDescription();
  des["compression"] = _compress->getJsonDescription();
  return des;
}
buffers::buffers(std::shared_ptr<hypercube> hyper, const dataType dataType,
                 std::shared_ptr<compress> comp,
                 std::shared_ptr<blocking> block,
                 std::shared_ptr<memoryUsage> mem) {
  _typ = dataType;
  _compress = comp;
  _blocking = block;
  _memory = mem;
  _hyper = hyper;

  if (_compress == nullptr) _compress = createDefaultCompress();
  if (_blocking == nullptr) _blocking = createDefaultBlocking();
  if (_memory == nullptr) _memory = createDefaultMemory();
  blockParams v = _blocking->makeBlocks(_hyper->getNs());
  createBuffers(UNDEFINED);
  _defaultStateSet = false;
}

void buffers::setDirectory(const std::string &dir, const bool createDirectory) {
  _directory = dir;
  if (createDirectory) {
    const int dir_err =
        mkdir(_directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err) {
      std::cerr << "Error creating directory, exists? DIR=" << dir << std::endl;
      exit(1);
    }
  }
  for (auto i = 0; i < _buffers.size(); i++) {
    _buffers[i].setName(_directory + std::string("/buf") + std::to_string(i));
  }
}

void buffers::createBuffers(const bufferState state) {
  std::vector<int> ns = _hyper->getNs();
  blockParams b = _blocking->makeBlocks(ns);

  for (int i = 0; i < b._ns.size(); i++) {
    _buffers.push_back(buffer(i, b._ns[i], b._fs[i], _compress, state));
  }

  _n123blocking = b._nblocking;
  _axisBlocking = b._axesBlock;
}
void buffers::updateMemory(const long change2) {
  bool done = false;
  long change = change2;
  while (!done) {
    std::shared_ptr<memoryReduce> a = _memory->changeBufferState(change);
    if (a->_toDisk.size() == 0 && a->_compress.size() == 0) {
      done = true;
    } else {
      long change = tbb::parallel_reduce(
          tbb::blocked_range<size_t>(0, a->_toDisk.size()), long(0),
          [&](const tbb::blocked_range<size_t> &r, long locChange) {
            for (size_t i = r.begin(); i != r.end(); ++i) {
              std::vector<int> n_w(7), f_w(7), j_w(7);
              locChange += _buffers[i].changeState(ON_DISK);
            }
            return locChange;
          },
          [](long a, long b) { return a + b; });
      change += tbb::parallel_reduce(
          tbb::blocked_range<size_t>(0, a->_compress.size()), long(0),
          [&](const tbb::blocked_range<size_t> &r, long locChange) {
            for (size_t i = r.begin(); i != r.end(); ++i) {
              std::vector<int> n_w(7), f_w(7), j_w(7);
              locChange += _buffers[i].changeState(CPU_COMPRESSED);
            }
            return locChange;
          },
          [](long a, long b) { return a + b; });
    }
  }
}
std::vector<int> buffers::parsedWindows(const std::vector<int> &nw,
                                        const std ::vector<int> &fw,
                                        const std::vector<int> &jw) {
  bool all = true;
  int nbig1 = 0;
  std::vector<int> bufSearch;
  std::vector<int> n1s;
  std::vector<int> ns = _hyper->getNs();
  std::vector<std::vector<bool>> patches;
  std::vector<int> first(1, 0);
  size_t ntot = 1;
  for (int i = ns.size(); i < nw.size(); i++) ns.push_back(1);
  for (int i = 0; i < nw.size(); i++) {
    bool fail = false;
    if (nw[i] < 1) {
      std::cerr << "axis[" << i << "] n < 1" << std::endl;
      fail = true;
    }
    if (fw[i] < 0) {
      std::cerr << "axis[" << i << "] f < 0" << std::endl;
      fail = true;
    }
    if (jw[i] < 1) {
      std::cerr << "axis[" << i << "] j < 1" << std::endl;
      fail = true;
    }
    if (fw[i] + jw[i] * (nw[i] - 1) > ns[i] - 1) {
      std::cerr << "axis[" << i << "] window out of range f=" << fw[i]
                << " j=" << jw[i] << " n=" << nw[i] << " ns=" << ns[i]
                << std::endl;
      fail = true;
    }
    if (fail) assert(1 == 2);
    std::vector<bool> axisP(_axisBlocking[i].size(), false);
    size_t ip = 0;
    int ibeg = 0;
    int iend = _axisBlocking[i][0];
    // Loop through all of window we are reading
    for (int iax = 0, ib = fw[i]; iax < nw[i]; iax++, ib += jw[i]) {
      // Go until we reach the patch on the axis containing the

      while (ip < _axisBlocking[i].size() && (ib < ibeg || ib >= iend)) {
        ibeg = ibeg + _axisBlocking[i][ip];
        ip++;
        if (ip < axisP.size()) iend = ibeg + _axisBlocking[i][ip];
      }
      if (ip < axisP.size()) {
        axisP[ip] = true;
      }
    }
    int ic = 0;
    for (size_t iax = 0; iax < axisP.size(); iax++) {
      if (axisP[iax]) ic++;
    }
    ntot *= ic;
    patches.push_back(axisP);
  }
  for (int i = patches.size(); i < 7; i++) {
    std::vector<bool> axisP(1, true);
    patches.push_back(axisP);
  }

  bufSearch.resize(ntot);
  int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ic = 0;
  for (int i6 = 0; i6 < patches[6].size(); i6++) {
    if (patches[6][i6]) {
      ib6 = i6 * _n123blocking[6];
      for (int i5 = 0; i5 < patches[5].size(); i5++) {
        if (patches[5][i5]) {
          ib5 = ib6 + _n123blocking[5] * i5;
          for (int i4 = 0; i4 < patches[4].size(); i4++) {
            if (patches[4][i4]) {
              ib4 = ib5 + _n123blocking[4] * i4;
              for (int i3 = 0; i3 < patches[3].size(); i3++) {
                if (patches[3][i3]) {
                  ib3 = ib4 + _n123blocking[3] * i3;
                  for (int i2 = 0; i2 < patches[2].size(); i2++) {
                    if (patches[2][i2]) {
                      ib2 = ib3 + _n123blocking[2] * i2;
                      for (int i1 = 0; i1 < patches[1].size(); i1++) {
                        if (patches[1][i1]) {
                          ib1 = ib2 + _n123blocking[1] * i1;
                          for (int i0 = 0; i0 < patches[0].size(); i0++) {
                            if (patches[0][i0]) {
                              bufSearch[ic++] = i0 + ib1;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return bufSearch;
}
void buffers::getWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                        const std::vector<int> &jw, void *buf) {
  bufferState state = CPU_DECOMPRESSED;
  if (_defaultStateSet) state = _defState;
  std::vector<int> pwind = parsedWindows(nw, fw, jw);
  std::vector<int> n(7, 1), f(7, 0), j(7, 1), fG(7, 0), nG(7, 1), f_w(7, 0),
      n_w(7, 1), j_w(7, 1), blockG(7, 1);
  for (auto i = 0; i < nw.size(); i++) n[i] = nw[i];
  for (auto i = 0; i < fw.size(); i++) f[i] = fw[i];
  for (auto i = 0; i < jw.size(); i++) j[i] = jw[i];
  int locChange = 0;
  // long change = tbb::parallel_reduce(
  //   tbb::blocked_range<size_t>(0, pwind.size()), long(0),
  //  [&](const tbb::blocked_range<size_t> &r, long locChange) {
  //   for (size_t i = r.begin(); i != r.end(); ++i) {
  for (size_t i = 0; i < pwind.size(); i++) {
    size_t pos =
        _buffers[pwind[i]].localWindow(n, f, j, n_w, f_w, j_w, nG, fG, blockG);

    locChange += _buffers[pwind[i]].getWindowCPU(n_w, f_w, j_w, nG, fG, blockG,
                                                 buf, state);
  }
  // return locChange;
  //  },
  //  [](long a, long b) { return a + b; });
}
void buffers::changeState(const bufferState state) {
  long change = tbb::parallel_reduce(
      tbb::blocked_range<size_t>(0, _buffers.size()), long(0),
      [&](const tbb::blocked_range<size_t> &r, long locChange) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
          locChange += _buffers[i].changeState(state);
        }
        return locChange;
      },
      [](long a, long b) { return a + b; });
}
void buffers::putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                        const std::vector<int> &jw, const void *buf) {
  bufferState state = CPU_DECOMPRESSED;
  if (_defaultStateSet) state = _defState;
  std::vector<int> pwind = parsedWindows(nw, fw, jw);
  std::vector<int> n(7, 1), f(7, 0), j(7, 1);
  for (auto i = 0; i < nw.size(); i++) n[i] = nw[i];
  for (auto i = 0; i < fw.size(); i++) f[i] = fw[i];
  for (auto i = 0; i < jw.size(); i++) j[i] = jw[i];
  int locChange = 0;
  // long change = tbb::parallel_reduce(
  //    tbb::blocked_range<size_t>(0, pwind.size()), long(0),
  //  [&](const tbb::blocked_range<size_t> &r, long locChange) {
  //  for (size_t i = r.begin(); i != r.end(); ++i) {
  for (size_t i = 0; i < pwind.size(); i++) {
    std::vector<int> n_w(7), f_w(7), j_w(7), nG(7), fG(7), blockG(7);
    size_t pos =
        _buffers[pwind[i]].localWindow(n, f, j, n_w, f_w, j_w, nG, fG, blockG);

    locChange += _buffers[pwind[i]].putWindowCPU(n_w, f_w, j_w, nG, fG, blockG,
                                                 buf, state);
  }

  //        return locChange;
  //    },
  //  [](long a, long b) { return a + b; });
}
// buffers(std::string diretory, std::shared_ptr<compress> comp = nullptr,
//      std::shared_ptr<blocking> block = nullptr);

// void readDescription();
// void writeDescription();
// void readBlocking();
// void writeBlocking();
