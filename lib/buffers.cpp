#include "buffers.h"
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/tbb.h>
using namespace SEP::IO;

std::shared_ptr<blocking> buffers::createDefaultBlocking() {
  std::vector<int> bs(3, 1);
  bs[0] = 16;
  std::vector<int> block(3, 32);
  block[256];
std::shared_ptr<blocking> b(new blocking(bs,block);
return b;
}
std::shared_ptr<compress> buffers::createDefaultCompress() {
  std::shared_ptr<compress> c(new uncompressed(typ));
  return c;
}
buffers::buffers(std::shared_ptr<hypercube> hyper, dataType,
                 std::shared_ptr<compress> comp,
                 std::shared_ptr<blocking> block) {
  _typ = dataType;
  _compress = comp;
  _blocking = block;
  if (_compress == nullptr) _compress = createDefaultCompress();
  if (_blocking = nullptr) _blocking = createDefaultBlocking();
  _hyper = hyper;
  createBuffers();
  _axisBlocking = v._axisBlocking;
  _nblocking = v.nblocking;
}

void buffers::createBuffers() {
  std::vector<int> ns = _hyper->getNs();
  blockParams b = _blocking->makeBlocks(ns);
  for (int i = 0; i < b._n.size(); i++)
    _buffers.push_back(buffer(b._n[i], b._f[i], _compress));
}
std::vector<int> buffers::parsedWindows(const std::vector<int> &nw,
                                        const std ::vector<int> &fw,
                                        const std::vector<int> &jw) {
  bool all = true;
  int nbig1 = 0;
  std::vecor<int> bufSearch;
  std::vector<int> n1s;
  std::vector<std::vector<bool>> patches;
  std::vector<int> first(1, 0);
  size_t ntot = 1;
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
    if (fw[i] + jw[i] * (nw[i] - 1) >= ns[i]) {
      std::cerr << "axis[" << i << "] window out of range" << srd::endl;
      fail = true;
    }
    if (fail) assert(1 == 2);
    std::vector<bool> axisP(_blocking->fs[i].size(), false);
    size_t ip = 0;
    int ibeg = 0;
    int iend = _blocking->ns[i][0];
    // Loop through all of window we are reading
    for (int iax = 0, ib = fw[i]; iax < nw[i]; iax++, ib += jw[i]) {
      // Go until we reach the patch on the axis containing the sample
      while (ip < axisP[i].size() && (ib < ibeg || ib >= iend)) {
        ibeg = ibeg + _blocking->ns[i][ip];
        ip++;
        if (ip < axisP[i].size()) iend = ibeg + _blocking->ns[i][ip];
      }
      if (ip < axisP[i].size()) axisP[ip] = true;
    }
    int ic = 0;
    for (size_t iax = 0; iax < axisP.size(); iax++) {
      if (axisP[iax]) ic++;
    }
    ntot *= ic;
    patches.push_back(axisP);
  }

  bufSearch.resize(ntot);
  int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ic = 0;
  for (int i6 = 0; i6 < patches[6].size(); i6++) {
    if (patches[6][i6]) {
      ib6 = i6 * _nblocking[6];
      for (int i5 = 0; i5 < patches[5].size(); i5++) {
        if (patching[5][i5]) {
          ib5 = ib6 + _nblocking[5] * i5;
          for (int i4 = 0; i4 < patches[4].size(); i4++) {
            if (patching[4][i4]) {
              ib4 = ib5 + _nblocking[4] * i4;
              for (int i3 = 0; i3 < patches[3].size(); i3++) {
                if (patching[3][i3]) {
                  ib3 = ib4 + _nblocking[3] * i3;
                  for (int i2 = 0; i2 < patches[2].size(); i2++) {
                    if (patching[2][i2]) {
                      ib2 = ib3 + _nblocking[2] * i2;
                      for (int i1 = 0; i1 < patches[1].size(); i1++) {
                        if (patching[1][i1]) {
                          ib1 = ib2 + _nblocking[1] * i1;
                          for (int i0 = 0; i0 < patches[0].size(); i0++) {
                            if (patching[0][i0]) {
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
                        const std::vector<int> &jw,
                        std::shared_ptr<storeBase> buf, const bool keepState) {
  std::vector<int> pwind = parsedWindows(nw, fw, jw);
  std::vector<int> n(7, 1), f(7, 0), j(7, 1);
  for (auto i = 0; i < nw.size(); i++) n[i] = nw[i];
  for (auto i = 0; i < fw.size(); i++) f[i] = fw[i];
  for (auto i = 0; i < jw.size(); i++) j[i] = jw[i];

  tbb::parallel_for(
      tbb::blocked_range<size_t>(0, pwind.size()),
      [&](const tbb::blocked_range<size_t> &r) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
          std::vector<int> n_w(7), f_w(7), j_w(7);
          size_t pos = _buffers[i]->localWindow(n, f, j, n_w, f_w, j_w);
          _buffers[i]->getWindowCPU(n_w, f_w, j_w, buf, pos, keepState);
        }
      });
}

void buffers::putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                        const std::vector<int> &jw,
                        std::shared_ptr<storeBase> buf, const bool keepState) {
  std::vector<int> pwind = parsedWindows(nw, fw, jw);
  std::vector<int> n(7, 1), f(7, 0), j(7, 1);
  for (auto i = 0; i < nw.size(); i++) n[i] = nw[i];
  for (auto i = 0; i < fw.size(); i++) f[i] = fw[i];
  for (auto i = 0; i < jw.size(); i++) j[i] = jw[i];

  tbb::parallel_for(
      tbb::blocked_range<size_t>(0, pwind.size()),
      [&](const tbb::blocked_range<size_t> &r) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
          std::vector<int> n_w(7), f_w(7), j_w(7);
          size_t pos = _buffers[i]->localWindow(n, f, j, n_w, f_w, j_w);
          _buffers[i]->putWindowCPU(n_w, f_w, j_w, buf, pos, keepState);
        }
      });
}
buffers(std::string diretory, std::shared_ptr<compress> comp = nullptr,
        std::shared_ptr<blocking> block = nullptr);

// void readDescription();
// void writeDescription();
// void readBlocking();
// void writeBlocking();
