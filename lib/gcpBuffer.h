#pragma once
#include <store.h>
#include <cassert>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "compress.h"
namespace SEP {
namespace IO {

class gcpBuffer : public buffer {
 public:
  gcpBuffer(const std::string name, const std::vector<int> &n,
            const std::vector<int> &f,
            std::shared_ptr<compress> comp);  // Read from file
  gcpBuffer(const int ibuf, const std::vector<int> &n,
            const std::vector<int> &f, std::shared_ptr<compress> comp,
            const bufferState state);

  virtual long long readBuffer();
  virtual long long writeBuffer(bool keepState = false);
};
}  // namespace IO
}  // namespace SEP
