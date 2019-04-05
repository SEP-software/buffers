#ifndef STORE_H
#define STORE_H 1
#include <cassert>
#include <complex>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "ioTypes.h"
namespace SEP {
namespace IO {
/*!
  Virtual storage object
*/
class storeBase {
 public:
  storeBase() { ; }
  //! Get data from class to buf
  /*!
    \param buf  Base class
  */
  virtual void getData(std::shared_ptr<storeBase> buf) const = 0;
  //! Put data into class from buf
  /*!
    \param buf  Base class
  */
  virtual void putData(const std::shared_ptr<storeBase> buf) = 0;
  //! Get window from global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nwG Number of samples in Global
    \param fwG First sample along each axis in Global
    \param jwG Jump between samples along each axis in Global
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) = 0;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nwG Number of samples in Global
    \param fwG First sample along each axis in Global
    \param jwG Jump between samples along each axis in Global
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, const void *bufIn) = 0;
  //! Clone a buffer
  virtual std::shared_ptr<storeBase> clone() const = 0;
  //! Return info about buffer
  virtual void info(const std::string &b) const { ; }
  //! Get size of a single element
  virtual size_t getElementSize() const = 0;
  //! Get the size of the dataset
  virtual size_t getSize() const = 0;
  //! Zero a buffer
  virtual void zero() = 0;
  //! Return a pointer to buffer
  virtual char *getPtr() = 0;
};

class storeInt : public storeBase {
 public:
  storeInt(const int n) { _buf.resize(n); }
  storeInt(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() override { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const override;
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, const void *bufIn) override;
  virtual size_t getElementSize() const override { return sizeof(int); }
  virtual size_t getSize() const override { return _buf.size(); }

 private:
  std::vector<int> _buf;
};

class storeByte : public storeBase {
 public:
  storeByte(const int n) { _buf.resize(n); }
  storeByte(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() override { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const override;
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, const void *bufIn) override;
  virtual size_t getElementSize() const override {
    return sizeof(unsigned char);
  }
  virtual size_t getSize() const override { return _buf.size(); }
  std::string toString() {
    return std::string(reinterpret_cast<const char *>(&_buf[0]), _buf.size());
  }
  void fromString(const std::string str) {
    const unsigned char *raw_memory =
        reinterpret_cast<const unsigned char *>(str.c_str());
    _buf = std::vector<unsigned char>(raw_memory, raw_memory + str.size());
  }

 private:
  std::vector<unsigned char> _buf;
};

class storeFloat : public storeBase {
 public:
  storeFloat(const int n) { _buf.resize(n); }
  storeFloat(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() override { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const override;
  virtual void info(const std::string &v) const override;
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, const void *bufIn) override;
  virtual size_t getElementSize() const override { return sizeof(float); }
  virtual size_t getSize() const override { return _buf.size(); }

 private:
  std::vector<float> _buf;
};

class storeDouble : public storeBase {
 public:
  storeDouble(const int n) { _buf.resize(n); }
  storeDouble(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() override { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const override;
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, const void *bufIn) override;
  virtual size_t getElementSize() const override { return sizeof(double); }
  virtual size_t getSize() const override { return _buf.size(); }

 private:
  std::vector<double> _buf;
};

class storeComplex : public storeBase {
 public:
  storeComplex(const int n) { _buf.resize(n); }
  storeComplex(const int n, void *buf);
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  char *getPtr() override { return (char *)_buf.data(); }
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  virtual void zero() override { _buf.resize(0); }
  virtual std::shared_ptr<storeBase> clone() const override;
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &nwG,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  virtual void putWindow(
      const std::vector<int> &nwL, const std::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nbL,
      const std::vector<int> &nwG, const std::vector<int> &fwG,
      const std::vector<int> &nbG, const void *bufIn) override;
  virtual size_t getElementSize() const override {
    return sizeof(std::complex<float>);
  }
  virtual size_t getSize() const override { return _buf.size(); }

 private:
  std::vector<std::complex<float>> _buf;
};

std::string returnStorageType(const std::shared_ptr<storeBase> bufIn);
std::shared_ptr<storeBase> returnStorage(const dataType state, const size_t n);
void throwError(const std::string myType, const std::string typeSent);

}  // namespace IO
}  // namespace SEP
#endif
