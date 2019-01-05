#pragma once
#include <store.h>
#include <cassert>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "buffer.h"
namespace SEP {
namespace IO {

class gcpBuffer : public buffer {
 public:
  gcpBuffer(const std::string &bucketName, const std::string name,
            const std::vector<int> &n, const std::vector<int> &f,
            std::shared_ptr<compress> comp);  // Read from file
  gcpBuffer(const std::string &bucketName, const std::vector<int> &n,
            const std::vector<int> &f, std::shared_ptr<compress> comp,
            const bufferState state);

  virtual long long readBuffer() override;
  virtual long long writeBuffer(bool keepState = false) override;
  virtual ~gcpBuffer() { ; }

 private:
  std::string _bucketName;
};
}  // namespace IO
}  // namespace SEP
