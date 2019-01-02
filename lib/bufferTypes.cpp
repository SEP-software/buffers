
#include "bufferTypes.h"
#include <cassert>
#include <iostream>
#ifdef USE_GCP
#include "gcpBuffer.h"
#endif
#include "fileBuffer.h"
using namespace SEP::IO;

bufferTypes::bufferTypes(const std::string &des) {
  _bufferType = des;
  setup();
}

bufferTypes::bufferTypes(const Json::Value &des) {
  if (des["bufferType"].isNull()) {
    std::cerr << std::string(
                     "trouble grabbing parameter bufferType from parameters")
              << std::endl;

    assert(1 == 2);
  }

  _bufferType = des["bufferType"].asString();
}
void bufferTypes::setup() {
  if (_bufferType == std::string("file")) {
  }
#ifdef USE_GCP
  else if (_bufferType == std::string("GCP")) {
  }
#endif
  else {
    std::cerr << "Unsupported buffer type with this build: " << _bufferType
              << std::endl;
    assert(1 == 2);
  }
}

std::shared_ptr<buffer> bufferTypes::getBufferObj(
    const std::string name, const std::vector<int> &n,
    const std::vector<int> &f, std::shared_ptr<compress> comp) {
  if (_bufferType == std::string("file")) {
    std::shared_ptr<fileBuffer> x(new fileBuffer(name, n, f, comp));
    return x;
  }
#ifdef USE_GCP
  else if (_bufferType == std::string("GCP")) {
    return std::make_shared<gcpBuffer>(name, n, f, comp);
  }
#endif
  else {
    std::cerr << "Unsupported buffer type with this build: " << _bufferType
              << std::endl;
    assert(1 == 2);
  }
}

std::shared_ptr<buffer> bufferTypes::getBufferObj(
    const std::vector<int> &n, const std::vector<int> &f,
    std::shared_ptr<compress> comp, const bufferState state) {
  if (_bufferType == std::string("file")) {
    return std::make_shared<fileBuffer>(n, f, comp, state);
  }
#ifdef USE_GCP
  else if (_bufferType == std::string("GCP")) {
    return std::make_shared<gcpBuffer>(n, f, comp, state);
  }
#endif
  else {
    std::cerr << "Unsupported buffer type with this build: " << _bufferType
              << std::endl;
    assert(1 == 2);
  }
}
