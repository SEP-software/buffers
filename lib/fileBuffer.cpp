#include <assert.h>
#include <fileBuffer.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include "SEPException.h"
using namespace SEP::IO;

bool checkErrorBitsIn(std::ifstream *f) {
  bool stop = false;

  if (f->eof()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    throw(SEPException(std::string(msg)));
  }

  if (f->fail()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    throw(SEPException(std::string(msg)));
    stop = true;
  }

  if (f->bad()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    throw(SEPException(std::string(msg)));
    stop = true;
  }

  return stop;
}
bool checkErrorBitsOut(std::ofstream *f) {
  bool stop = false;

  if (f->eof()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    throw(SEPException(std::string(msg)));
  }

  if (f->fail()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    throw(SEPException(std::string(msg)));
    stop = true;
  }

  if (f->bad()) {
    char msg[500];
    strerror_r(errno, msg, 500);
    throw(SEPException(std::string(msg)));
    stop = true;
  }

  return stop;
}

long long fileBuffer::writeBuffer(bool keepState) {
  long long oldSize = _buf->getSize();

  std::shared_ptr<storeBase> buf;
  bufferState restore;
  if (_bufferState == UNDEFINED)
    throw SEPException(
        std::string("Trying to write buffer that hasn't been created"));
  if (_bufferState == ON_DISK) return 0;
  if (keepState) {
    restore = _bufferState;
    buf = _buf->clone();
  }

  changeState(CPU_COMPRESSED);

  if (!_nameSet)
    throw SEPException(
        std::string("Trying to write buffer when name has not been set"));
  std::ofstream out(_name, std::ofstream::binary);
  out.write(_buf->getPtr(), _buf->getSize());

  out.close();

  if (keepState) {
    _buf = buf;
    _bufferState = restore;
  } else {
    _bufferState = ON_DISK;
  }
  return _buf->getSize() - oldSize;
}

long long fileBuffer::readBuffer() {
  long long oldSize = _buf->getSize();
  _modified = false;
  /*Only need to do something if sitting on disk*/
  if (_bufferState == ON_DISK) {
    if (!_nameSet)
      throw SEPException(
          std::string("Trying to write buffer when name has not been set"));
    std::ifstream in(_name, std::iostream::in | std::iostream::binary);

    if (!in)
      throw SEPException(std::string("Trouble opening stream for reading"));
    in.seekg(0, std::iostream::end);
    int nelemFile = in.tellg();
    in.seekg(0, std::iostream::beg);

    std::shared_ptr<storeByte> x(new storeByte(nelemFile));
    _buf = x;
    in.read(_buf->getPtr(), nelemFile);
    char *xx = (char *)_buf->getPtr();

    _bufferState = CPU_COMPRESSED;
    in.close();
  }
  if (_bufferState == UNDEFINED)
    throw SEPException(std::string("bufferstate is undefined"));
  return _buf->getSize() - oldSize;
}

fileBuffer::fileBuffer(const std::string name, const std::vector<int> &n,
                       const std::vector<int> &f,
                       std::shared_ptr<compress> comp) {
  setLoc(n, f);
  setName(name);
  setCompress(comp);
  setBufferState(ON_DISK);
}
fileBuffer::fileBuffer(const std::vector<int> &n, const std::vector<int> &f,
                       std::shared_ptr<compress> comp,
                       const bufferState state) {
  setLoc(n, f);
  setCompress(comp);
  setBufferState(state);
  createStorageBuffer();
  _compress = comp;
}
