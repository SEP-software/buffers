#ifndef BLOCKING_H
#define BLOCKING_H 1
#include <memory>
#include <vector>
namespace SEP {
namespace IO {
class blockParams() {
 public:
  blockParams() { ; }
  std::vector<std::vector<int>> _fs, _ns;
  std::vector < std::vector<int> _blocks;
}

class blocking {
 public:
  blocking(const std::vector<int> &blocksize, std::vector<int> nb) {
    _blocksize = blocksize;
    _nb = n;
    checkLogicBlocking();
  }
  void checkLogicBlocking();
  virtual std::vector<std::vector<int>> makeBlocks(const std::vector<int> &n);
  std::vector<std::vector<int>> blockAxis(const std::vector<int> &n);

 private:
  std::vector<int> _nb, _blocksize;
  compressDataType _typ;
};

}  // namespace IO
}  // namespace SEP
#endif