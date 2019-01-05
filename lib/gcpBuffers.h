#pragma once
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffers.h"
#include "compress.h"
#include "gcpBuffer.h"
#include "hypercube.h"
#include "memoryUsage.h"

namespace SEP {
namespace IO {
class gcpBuffers : public buffers {
 public:
  gcpBuffers(const std::shared_ptr<hypercube>, const dataType dataType,
             std::shared_ptr<compress> comp = nullptr,
             std::shared_ptr<blocking> block = nullptr,
             std::shared_ptr<memoryUsage> mem = nullptr);
  gcpBuffers(const std::shared_ptr<hypercube> hyper, const std::string dir,
             const Json::Value &jsonArgs,
             std::shared_ptr<memoryUsage> mem = nullptr);
  virtual void setName(const std::string &dir, const bool create) override;
  virtual void createBuffers(const bufferState state) override;
  std::string getEnvVar(std::string const &key,
                        const std::string &defaultV) const {
    char *val = getenv(key.c_str());
    return val == NULL ? defaultV : std::string(val);
  }

 private:
  std::string _projectID, _region;
};
}  // namespace IO
}  // namespace SEP
