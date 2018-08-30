#ifndef STORE_H
#define STORE_H 1
#include <cassert>
#include <complex>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "ioTypes.h"
namespace SEP {
namespace IO {

class storeBase {
 public:
  storeBase() { ; }
  virtual void getData(std::shared_ptr<storeBase> buf) const = 0;
  virtual void putData(const std::shared_ptr<storeBase> buf) = 0;
  virtual void getWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) = 0;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) = 0;
  virtual std::shared_ptr<storeBase> clone() const = 0;
  virtual size_t getElementSize() const = 0;
  virtual size_t getSize() const = 0;
  virtual void zero() = 0;
  virtual char *getPtr() = 0;
};

class storeInt : public storeBase {
 public:
  storeInt(const int n) { _buf.resize(n); }
  storeInt(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const;
  virtual void getWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual size_t getElementSize() const { return sizeof(int); }
  virtual size_t getSize() const { return _buf.size(); }

 private:
  std::vector<int> _buf;
};

class storeByte : public storeBase {
 public:
  storeByte(const int n) { _buf.resize(n); }
  storeByte(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const;
  virtual void getWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual size_t getElementSize() const { return sizeof(unsigned char); }
  virtual size_t getSize() const { return _buf.size(); }

 private:
  std::vector<unsigned char> _buf;
};

class storeFloat : public storeBase {
 public:
  storeFloat(const int n) { _buf.resize(n); }
  storeFloat(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const;
  virtual void getWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual size_t getElementSize() const { return sizeof(float); }
  virtual size_t getSize() const { return _buf.size(); }

 private:
  std::vector<float> _buf;
};

class storeDouble : public storeBase {
 public:
  storeDouble(const int n) { _buf.resize(n); }
  storeDouble(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const;
  virtual void getWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual size_t getElementSize() const { return sizeof(double); }
  virtual size_t getSize() const { return _buf.size(); }

 private:
  std::vector<double> _buf;
};

class storeComplex : public storeBase {
 public:
  storeComplex(const int n) { _buf.resize(n); }
  storeComplex(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const;
  virtual void getWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, std::shared_ptr<storeBase> bufIn) override;
  virtual size_t getElementSize() const { return sizeof(std::complex<float>); }
  virtual size_t getSize() const { return _buf.size(); }

 private:
  std::vector<std::complex<float>> _buf;
};

std::string returnStorageType(const std::shared_ptr<storeBase> bufIn);
std::shared_ptr<storeBase> returnStorage(const dataType state, const size_t n);
void throwError(const std::string myType, const std::string typeSent);

}  // namespace IO
}  // namespace SEP
#endif
