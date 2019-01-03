#include "fileBuffers.h"
#include <sys/stat.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

#include <tbb/tbb.h>
#include "compressTypes.h"
#include "memoryAll.h"
#include "nocompress.h"
#include "simpleMemoryLimit.h"
using namespace SEP::IO;

fileBuffers::fileBuffers(const std::shared_ptr<hypercube> hyper,
                         const std::string dir, const Json::Value &des,
                         std::shared_ptr<memoryUsage> mem) {
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
  }

  _memory = mem;
  if (!_memory) {
    _memory = createDefaultMemory();
  }

  SEP::IO::compressTypes ct = compressTypes(des["compression"]);

  _compress = ct.getCompressionObj();

  _defaultStateSet = false;

  createBuffers(ON_DISK);
  setName(dir, false);
}
fileBuffers::fileBuffers(std::shared_ptr<hypercube> hyper,
                         const dataType dataType,
                         std::shared_ptr<compress> comp,
                         std::shared_ptr<blocking> block,
                         std::shared_ptr<memoryUsage> mem) {
  _typ = dataType;
  _compress = comp;
  _blocking = block;
  _memory = mem;
  _hyper = hyper;
  if (_compress == nullptr) _compress = createDefaultCompress();
  if (_blocking == nullptr) _blocking = blocking::createDefaultBlocking(_hyper);
  if (_memory == nullptr) _memory = createDefaultMemory();

  blockParams v = _blocking->makeBlocks(_hyper->getNs());
  createBuffers(UNDEFINED);
  _defaultStateSet = false;
}
std::shared_ptr<bufferTypes> buffers::createDefaultBufferTypes() {
  _bufferT = std::make_shared<bufferTypes>(std::string("file"));
}

void fileBuffers::setName(const std::string &dir, const bool create) {
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
    _buffers[i]->setName(_directory + std::string("/buf") + std::to_string(i));
  }
}
void fileBuffers::createBuffers(const bufferState state) {
  std::vector<int> ns = _hyper->getNs();
  blockParams b = _blocking->makeBlocks(ns);
  assert(_bufferT);
  for (int i = 0; i < b._ns.size(); i++) {
    _buffers.push_back(
        std::make_shared<fileBuffer>(b._ns[i], b._fs[i], _compress, state));
  }

  _n123blocking = b._nblocking;
  _axisBlocking = b._axesBlock;
}