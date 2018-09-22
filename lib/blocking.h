#ifndef BLOCKING_H
#define BLOCKING_H 1
#include <json.h>
#include <memory>
#include <vector>
#include "hypercube.h"
namespace SEP {
namespace IO {
class blockParams {
 public:
  blockParams() { ; }
  std::vector<std::vector<int>> _fs, _ns;
  // std::vector<std::vector<int>> _blocks;
  std::vector<int> _nblocking;
  std::vector<std::vector<int>> _axesBlock;
};

class blocking {
 public:
  blocking(const std::vector<int> &blocksize, std::vector<int> &nb) {
    _blocksize = blocksize;
    _nb = nb;
    checkLogicBlocking();
  }
  blocking(const Json::Value &jsonArgs);
  blockParams makeBlocks(const std::vector<int> &n);
  void checkLogicBlocking();
  std::vector<std::vector<int>> blockAxis(const std::vector<int> &n);
  Json::Value getJsonDescription();
  static std::shared_ptr<blocking> createDefaultBlocking(
      std::shared_ptr<SEP::hypercube> hyper);

 private:
  std::vector<int> _nb, _blocksize;
  // compressDataType _typ;
};

}  // namespace IO
}  // namespace SEP
#endif