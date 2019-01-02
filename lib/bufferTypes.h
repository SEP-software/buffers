#ifndef BUFFER_TYPES
#define BUFFER_TYPES 1
#include <json.h>
#include <complex>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include "buffer.h"
namespace SEP {
namespace IO {

class bufferTypes {
 public:
  bufferTypes(const Json::Value &val);
  bufferTypes(const std::string &val);
  void setup();
  std::shared_ptr<buffer> getBufferObj(const std::string name,
                                       const std::vector<int> &n,
                                       const std::vector<int> &f,
                                       std::shared_ptr<compress> comp);
  std::shared_ptr<buffer> getBufferObj(const std::vector<int> &n,
                                       const std::vector<int> &f,
                                       std::shared_ptr<compress> comp,
                                       const bufferState state);

 private:
  std::string _bufferType = "file";
};

}  // namespace IO
}  // namespace SEP
#endif