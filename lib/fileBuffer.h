#pragma once
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "buffer.h"
namespace SEP {
namespace IO {

class fileBuffer : public buffer {
 public:
  fileBuffer(const std::string name, const std::vector<int> &n,
             const std::vector<int> &f,
             std::shared_ptr<compress> comp);  // Read from file
  fileBuffer(const std::vector<int> &n, const std::vector<int> &f,
             std::shared_ptr<compress> comp, const bufferState state);

  virtual long long readBuffer() override;
  virtual long long writeBuffer(bool keepState = false) override;
  virtual ~fileBuffer() { ; }
};
}  // namespace IO
}  // namespace SEP
