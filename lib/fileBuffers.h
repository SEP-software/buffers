#ifndef FILE_BUFFERS_H
#define FILE_BUFFERS_H 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffers.h"
#include "compress.h"
#include "fileBuffer.h"
#include "hypercube.h"
#include "memoryUsage.h"
namespace SEP {
namespace IO {
class fileBuffers : public buffers {
 public:
  fileBuffers(const std::shared_ptr<hypercube>, const dataType dataType,
              std::shared_ptr<compress> comp = nullptr,
              std::shared_ptr<blocking> block = nullptr,
              std::shared_ptr<memoryUsage> mem = nullptr);
  fileBuffers(const std::shared_ptr<hypercube> hyper, const std::string dir,
              const Json::Value &jsonArgs,
              std::shared_ptr<memoryUsage> mem = nullptr);
  virtual void setName(const std::string &dir, const bool create) override;
  virtual void createBuffers(const bufferState state) override;
};
}  // namespace IO
}  // namespace SEP
#endif
