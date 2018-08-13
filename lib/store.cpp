#include <assert.h>
#include <store.h>
using namespace SEP::IO;

floatStore::floatStore(const int n) { _buf.resize(n); }
void floatStore::getData(std::shared_ptr<store> buf) const {
  std::shared_ptr<floatStore> b = std::dynamic_pointer_cast<floatStore>(buf);
  assert(b);
  assert(b->_buf.size() == _buf.size());
  memcpy(b->_buf.data(), _buf.data(), _buf.size() * sizeof(float));
}
void floatStore::putData(const std::shared_ptr<store> buf) {
  const std::shared_ptr<floatStore> b =
      std::dynamic_pointer_cast<floatStore>(buf);
  assert(b);
  assert(b->_buf.size() == _buf.size());
  memcpy(_buf.data(), b->_buf.data(), _buf.size() * sizeof(float));
}

std::shared_ptr<store> floatStore::clone() const {
  std::shared_ptr<floatStore> m(new floatStore((int)_buf.size()));
  memcpy(m->_buf.data(), _buf.data(), _buf.size() * sizeof(float));

  return m;
}
byteStore::byteStore(const int n) { _buf.resize(n); }
void byteStore::getData(std::shared_ptr<store> buf) const {
  std::shared_ptr<byteStore> b = std::dynamic_pointer_cast<byteStore>(buf);
  assert(b);
  assert(b->_buf.size() == _buf.size());
  memcpy(b->_buf.data(), _buf.data(), _buf.size() * sizeof(char));
}
void byteStore::putData(const std::shared_ptr<store> buf) {
  const std::shared_ptr<byteStore> b =
      std::dynamic_pointer_cast<byteStore>(buf);
  assert(b);
  assert(b->_buf.size() == _buf.size());
  memcpy(_buf.data(), b->_buf.data(), _buf.size() * sizeof(char));
}

std::shared_ptr<store> byteStore::clone() const {
  std::shared_ptr<byteStore> m(new byteStore((int)_buf.size()));
  memcpy(m->_buf.data(), _buf.data(), _buf.size() * sizeof(char));
  return m;
}

complexStore::complexStore(const int n) { _buf.resize(n); }
void complexStore::getData(std::shared_ptr<store> buf) const {
  std::shared_ptr<complexStore> b =
      std::dynamic_pointer_cast<complexStore>(buf);
  assert(b);
  assert(b->_buf.size() == _buf.size());
  memcpy(b->_buf.data(), _buf.data(),
         _buf.size() * sizeof(std::complex<float>));
}
void complexStore::putData(const std::shared_ptr<store> buf) {
  const std::shared_ptr<complexStore> b =
      std::dynamic_pointer_cast<complexStore>(buf);
  assert(b);
  assert(b->_buf.size() == _buf.size());
  memcpy(_buf.data(), b->_buf.data(),
         _buf.size() * sizeof(std::complex<float>));
}

std::shared_ptr<store> complexStore::clone() const {
  std::shared_ptr<complexStore> m(new complexStore((int)_buf.size()));
  memcpy(m->_buf.data(), _buf.data(),
         _buf.size() * sizeof(std::complex<float>));
  return m;
}
