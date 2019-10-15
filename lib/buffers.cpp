#include "buffers.h"
#include <sys/stat.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/tbb.h>
#include <functional>
#include <numeric>

#include <future>
#include "compressTypes.h"
#include "memoryAll.h"
#include "nocompress.h"
#include "simpleMemoryLimit.h"
using namespace SEP::IO;

buffers::buffers() {
  char *val = getenv("ioThreads");
  _ioThreads = 60;
  if (val != NULL) _ioThreads = atoi(val);
}

std::shared_ptr<compress> buffers::createDefaultCompress() {
  std::shared_ptr<compress> c(new noCompression(_typ));
  return c;
}
std::shared_ptr<memoryUsage> buffers::createDefaultMemory() {
  std::shared_ptr<memoryUsage>

      /*
  c(new simpleMemoryLimit((long long)(1024 * 1024) *
                          (long long)(4 * 1024)));
                          */
      c(new memoryAll());

  return c;
}

void getValsLocation(const std::vector<long long> locs, const void *buf) {}

Json::Value buffers::getDescription() {
  Json::Value des;
  if (!_blocking)
    throw SEPException(
        std::string("Blocking undefined can't return description"));

  if (!_compress)
    throw SEPException(
        std::string("Compression undefined can't return description"));

  des["blocking"] = _blocking->getJsonDescription();

  des["compression"] = _compress->getJsonDescription();

  return des;
}

void buffers::updateMemory(const long change2) {
  bool done = false;
  long change = change2;
  while (!done) {
    std::shared_ptr<memoryReduce> a = _memory->changeBufferState(change);
    if (a->_toDisk.size() == 0 && a->_compress.size() == 0) {
      done = true;
    } else {
      long long sz = 0;
      long long ibuf = 0;
      /*
      std::mutex mtx;

      std::vector<std::thread> ioT(_ioThreads);

      auto func1 = [&]() {
        bool done = false;
        while (!done) {
          long long iuser;
          {
            std::lock_guard<std::mutex> lock(mtx);
            ibuf++;
            iuser = ibuf;
          }
          if (iuser < a->_toDisk.size()) {
            long long ch = _buffers[a->_toDisk[iuser]]->changeState(ON_DISK);

            {
              std::lock_guard<std::mutex> lock(mtx);
              change += ch;
            }
          } else
            done = true;
        }
      };

      auto func2 = [&]() {
        bool done = false;
        while (!done) {
          long long iuser;
          {
            std::lock_guard<std::mutex> lock(mtx);
            iuser = ibuf;
            ibuf++;
          }
          if (iuser < a->_compress.size()) {
            long long ch =
                _buffers[a->_compress[iuser]]->changeState(CPU_COMPRESSED);

            {
              std::lock_guard<std::mutex> lock(mtx);
              change += ch;
            }
          } else
            done = true;
        }
      };

      for (auto i = 0; i < ioT.size(); i++) {
        ioT[i] = std::thread(func1);
      }
      for (auto i = 0; i < ioT.size(); i++) ioT[i].join();

      for (auto i = 0; i < ioT.size(); i++) {
        ioT[i] = std::thread(func2);
      }
      for (auto i = 0; i < ioT.size(); i++) ioT[i].join();

        */
      // Async version
      std::vector<std::future<long long>> changes;

      for (auto i = 0; i < a->_toDisk.size(); i++)
        changes.push_back(std::async(
            std::launch::async,
            [&](int i) {
              return (long long)_buffers[a->_toDisk[i]]->changeState(ON_DISK);
            },
            i));
      change = 0;
      for (auto &n : changes) {
        try {
          change += n.get();
        } catch (std::exception e) {
          throw e;
        }
      }

      /* TBB implementation for debugging
    }
  long change = tbb::parallel_reduce(
      tbb::blocked_range<size_t>(0, a->_toDisk.size()), long(0),
      [&](const tbb::blocked_range<size_t> &r, long locChange) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
          locChange += _buffers[a->_toDisk[i]]->changeState(ON_DISK);
        }
        return locChange;
      },
      [](long a, long b) { return a + b; });

      */

      /*
            change += tbb::parallel_reduce(
                tbb::blocked_range<size_t>(0, a->_compress.size()), long(0),
                [&](const tbb::blocked_range<size_t> &r, long locChange) {
                  for (size_t i = r.begin(); i != r.end(); ++i) {
                    locChange +=
                        _buffers[a->_compress[i]]->changeState(CPU_COMPRESSED);
                  }
                  return locChange;
                },
                [](long a, long b) { return a + b; });
          }
        }
        */

      std::vector<std::future<long long>> changes2;

      for (auto i = 0; i < a->_compress.size(); i++)
        changes2.push_back(std::async(
            std::launch::async,
            [&](int i) {
              return (long long)_buffers[a->_compress[i]]->changeState(ON_DISK);
            },
            i));
      change = 0;
      for (auto &n : changes2) change += n.get();
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
  for (int i = 0; i < std::min(7, (int)nw.size()); i++) {
    bool fail = false;
    if (nw[i] < 1)
      throw SEPException(std::string("axis[") + std::to_string(i) +
                         std::string("] n < 1"));

    if (fw[i] < 0)
      throw SEPException(std::string("axis[") + std::to_string(i) +
                         std::string("] f < 0"));

    if (jw[i] < 1)
      throw SEPException(std::string("axis[") + std::to_string(i) +
                         std::string("] j < 1"));

    if (fw[i] + jw[i] * (nw[i] - 1) > ns[i] - 1)
      throw SEPException(
          std::string("axis[") + std::to_string(i) +
          std::string("] window out of range f=") + std::to_string(fw[i]) +
          std::string(" j=") + std::to_string(jw[i]) + std::string(" n=") +
          std::to_string(nw[i]) + std::string(" ns=") + std::to_string(ns[i]));

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

  std::vector<int> n(7, 1), f(7, 0), j(7, 1);
  for (auto i = 0; i < std::min(7, (int)nw.size()); i++) n[i] = nw[i];
  for (auto i = 0; i < std::min(7, (int)fw.size()); i++) f[i] = fw[i];
  for (auto i = 0; i < std::min(7, (int)jw.size()); i++) j[i] = jw[i];
  if (!_memory) throw SEPException(std::string("Memory has not been set"));

  _memory->updateRecentBuffers(pwind);
  /*
  long long change = 0;
  long long ibuf = 0;


  std::mutex mtx;

  std::vector<std::thread> ioT(_ioThreads);

  auto func = [&]() {
    bool done = false;
    while (!done) {
      long long iuser;
      {
        std::lock_guard<std::mutex> lock(mtx);
        iuser = ibuf;
        ibuf++;
      }
      if (iuser < pwind.size()) {
        std::vector<int> fG(7, 0), nG(7, 1), f_w(7, 0), n_w(7, 1), j_w(7, 1),
            blockG(7, 1);
        size_t pos = _buffers[pwind[iuser]]->localWindow(n, f, j, n_w, f_w, j_w,
                                                         nG, fG, blockG);

        long long ch = (long long)_buffers[pwind[iuser]]->getWindowCPU(
            n_w, f_w, j_w, nG, fG, blockG, buf, state);

        {
          std::lock_guard<std::mutex> lock(mtx);
          change += ch;
        }
      } else
        done = true;
    }
  };
  for (auto i = 0; i < ioT.size(); i++) {
    ioT[i] = std::thread(func);
  }
  for (auto i = 0; i < ioT.size(); i++) ioT[i].join();
*/
  /*Async version*/
  std::vector<std::future<long long>> changes;
  for (auto i = 0; i < pwind.size(); i++)
    changes.push_back(
        std::async(std::launch::async,
                   [&](int i) {
                     std::vector<int> fG(7, 0), nG(7, 1), f_w(7, 0), n_w(7, 1),
                         j_w(7, 1), blockG(7, 1);
                     size_t pos = _buffers[pwind[i]]->localWindow(
                         n, f, j, n_w, f_w, j_w, nG, fG, blockG);

                     return (long long)_buffers[pwind[i]]->getWindowCPU(
                         n_w, f_w, j_w, nG, fG, blockG, buf, state);
                   },
                   i));
  long long change = 0;
  for (auto &n : changes) change += n.get();
  /* */
  /* TBB implementation
    long change = tbb::parallel_reduce(
        tbb::blocked_range<size_t>(0, pwind.size()), long(0),
        [&](const tbb::blocked_range<size_t> &r, long locChange) {
          for (size_t i = r.begin(); i != r.end(); ++i) {
            // for (size_t i = 0; i < pwind.size(); i++) {
            std::vector<int> fG(7, 0), nG(7, 1), f_w(7, 0), n_w(7, 1), j_w(7,
    1), blockG(7, 1); size_t pos = _buffers[pwind[i]]->localWindow(n, f, j,
    n_w, f_w, j_w, nG, fG, blockG);

            locChange += _buffers[pwind[i]]->getWindowCPU(n_w, f_w, j_w, nG,
    fG, blockG, buf, state);
          }
          return locChange;
        },
        [](long a, long b) { return a + b; });
        */

  updateMemory(change);
}
void buffers::changeState(const bufferState state) {
  /* Thread version */

  //	  /*

  std::vector<std::future<long>> changes;

  for (auto i = 0; i < _buffers.size(); i++) {
    changes.push_back(
        std::async(std::launch::async,
                   [&](int i) { return _buffers[i]->changeState(state); }, i));
  }

  long long change = 0;
  for (auto &n : changes) change += n.get();

  /*
  long long ibuf = 0;

   std::mutex mtx;

  std::vector<std::thread> ioT(_ioThreads);

  auto func = [&]() {
    bool done = false;
    while (!done) {
      long long iuser;
      {
        std::lock_guard<std::mutex> lock(mtx);
        iuser = ibuf;
        ibuf++;
      }
      if (iuser < _buffers.size()) {
        long long ch = _buffers[iuser]->changeState(state);
        {
          std::lock_guard<std::mutex> lock(mtx);
          change += ch;
        }
      } else
        done = true;
    }
  };
  for (auto i = 0; i < ioT.size(); i++) {
    ioT[i] = std::thread(func);
  }
  for (auto i = 0; i < ioT.size(); i++) ioT[i].join();
    */
  //   */
  /*
 long long change = 0;
 long i = 0;
 for (auto &n : changes) {
   try {
     change += n.get();
   } catch (std::exception &e) {
     throw(e);
   }
 }

 updateMemory(change);

 */
  //	TBB implementation
  // /*

  /*

long change = tbb::parallel_reduce(
tbb::blocked_range<size_t>(0, _buffers.size()), long(0),
[&](const tbb::blocked_range<size_t> &r, long locChange) {
  for (size_t i = r.begin(); i != r.end(); ++i) {
    locChange += _buffers[i]->changeState(state);
  }
  return locChange;
},
[](long a, long b) { return a + b; });
*/
  /* Serial implementation */
  /*
      long change = 0;
  for (size_t i = 0; i < _buffers.size(); i++) {
    change += _buffers[i]->changeState(state);
  }
  */
  //}
  updateMemory(change);
}
void buffers::putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                        const std::vector<int> &jw, const void *buf) {
  bufferState state = CPU_DECOMPRESSED;
  if (_defaultStateSet) state = _defState;
  std::vector<int> pwind = parsedWindows(nw, fw, jw);
  std::vector<int> n(7, 1), f(7, 0), j(7, 1);
  _memory->updateRecentBuffers(pwind);
  for (auto i = 0; i < std::min(7, (int)nw.size()); i++) n[i] = nw[i];
  for (auto i = 0; i < std::min(7, (int)fw.size()); i++) f[i] = fw[i];
  for (auto i = 0; i < std::min(7, (int)jw.size()); i++) j[i] = jw[i];

  /* Thread version */
  /*
    long long change = 0;
    long long ibuf = 0;
    std::mutex mtx;

        std::vector<std::thread> ioT(_ioThreads);

      auto func = [&]() {
        bool done = false;
        while (!done) {
          long long iuser;
          {
            std::lock_guard<std::mutex> lock(mtx);
            iuser = ibuf;
            ibuf++;
          }
          if (iuser < pwind.size()) {
            std::vector<int> n_w(7), f_w(7), j_w(7), nG(7), fG(7), blockG(7);
            size_t pos = _buffers[pwind[iuser]]->localWindow(n, f, j, n_w, f_w,
      j_w, nG, fG, blockG);

            long long ch = _buffers[pwind[iuser]]->putWindowCPU(
                n_w, f_w, j_w, nG, fG, blockG, buf, state);

            {
              std::lock_guard<std::mutex> lock(mtx);
              change += ch;
            }
          } else
            done = true;
        }
      };
      for (auto i = 0; i < ioT.size(); i++) {
        ioT[i] = std::thread(func);
      }
      for (auto i = 0; i < ioT.size(); i++) ioT[i].join();
    */
  // Async version

  // int locChange = 0;

  std::vector<std::future<long long>> changes;

  for (auto i = 0; i < _buffers.size(); i++)
    changes.push_back(std::async(
        std::launch::async,
        [&](int i) {
          std::vector<int> n_w(7), f_w(7), j_w(7), nG(7), fG(7), blockG(7);
          size_t pos = _buffers[pwind[i]]->localWindow(n, f, j, n_w, f_w, j_w,
                                                       nG, fG, blockG);

          return (long long)_buffers[pwind[i]]->putWindowCPU(
              n_w, f_w, j_w, nG, fG, blockG, buf, state);
        },
        i));
  long long change = 0;
  for (auto &n : changes) change += n.get();

  /*

    long change = tbb::parallel_reduce(
        tbb::blocked_range<size_t>(0, pwind.size()), long(0),
        [&](const tbb::blocked_range<size_t> &r, long locChange) {
          for (size_t i = r.begin(); i != r.end(); ++i) {
            // for (size_t i = 0; i < pwind.size(); i++) {
            std::vector<int> n_w(7), f_w(7), j_w(7), nG(7), fG(7),
    blockG(7); size_t pos = _buffers[pwind[i]]->localWindow(n, f, j, n_w,
    f_w, j_w, nG, fG, blockG);

            locChange = _buffers[pwind[i]]->putWindowCPU(n_w, f_w, j_w, nG,
    fG, blockG, buf, state);
          }

          return locChange;
        },
        [](long a, long b) { return a + b; });
      */
  updateMemory(change);
}
