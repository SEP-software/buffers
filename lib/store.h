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
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) = 0;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
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
/*!
 Storage object for integer data
*/

class storeInt : public storeBase {
 public:
  //! Create an integer storage buffer
  /*!
    \param n Size of the buffer
  */
  storeInt(const size_t n) {
    cleanMemory();
    _buf = new int[n];
  }
  //! Create an integer storage buffer
  /*!
    \param n Size of the buffer
    \param buf Data to store in buffer
  */
  storeInt(const size_t n, void *buf);
  //! Copy the contents of buffer into buf
  /*!
    \param buf Data to store contents of  buffer into
  */
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  //! Return a pointer to buffer

  char *getPtr() override { return (char *)_buf.data(); }
  //! Put data into class from buf
  /*!
    \param buf  Base class
  */
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  //! Zero a buffer
  virtual void zero() override { cleanMemory(); }
  //! Clone a buffer

  virtual std::shared_ptr<storeBase> clone() const override;
  //! Get window from global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG,
                         const void *bufIn) override;
  //! Get size of a single element

  virtual size_t getElementSize() const override { return sizeof(int); }
  //! Get the size of the dataset

  virtual size_t getSize() const override { return _buf.size(); }

  //! Clean memory
  inline void cleanMemory() {
    if (_buf) {
      delete[] _buf;
      _buf = NULL;
    }
  }

  //! Destructor
  virtual ~storeInt() { cleanMemory(); }

 private:
  int *_buf = NULL;
};
/*!
 Storage object for byte data
*/
class storeByte : public storeBase {
 public:
  //! Create an byte storage buffer
  /*!
    \param n Size of the buffer
  */
  storeByte(const size_t n) {
    cleanMemory();
    _buf = new unsigned char[n];
  }
  //! Create an byte storage buffer
  /*!
    \param n Size of the buffer
    \param buf Data to store in buffer
  */
  storeByte(const size_t n, void *buf);
  //! Copy the contents of buffer into buf
  /*!
    \param buf Data to store contents of  buffer into
  */
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  //! Return a pointer to buffer

  char *getPtr() override { return (char *)_buf.data(); }
  //! Put data into class from buf
  /*!
    \param buf  Base class
  */
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  //! Zero a buffer

  virtual void zero() override { cleanMemory(); }
  //! Clone a buffer

  virtual std::shared_ptr<storeBase> clone() const override;
  //! Get window from global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG,
                         const void *bufIn) override;
  //! Get size of a single element

  virtual size_t getElementSize() const override {
    return sizeof(unsigned char);
  }
  /*!
     Resize buffer

     \param New size
  */
  void resize(size_t nsz) { cleanMemory(); }

  //! Get the size of the dataset
  virtual size_t getSize() const override { return _buf.size(); }
  //! Cast buffer to string
  std::string toString() {
    return std::string(reinterpret_cast<const char *>(&_buf[0]), _buf.size());
  }
  //! Reinterpret string as a buffer
  /*!
    \param str Reintepret string as buffer
    */
  void fromString(const std::string str) {
    const unsigned char *raw_memory =
        reinterpret_cast<const unsigned char *>(str.c_str());
    _buf = std::vector<unsigned char>(raw_memory, raw_memory + str.size());
  }
  //! Clean memory
  inline void cleanMemory() {
    if (_buf) {
      delete[] _buf;
      _buf = NULL;
    }
  }

  //! Destructor
  virtual ~storeByte() { cleanMemory(); }

 private:
  unsigned char *_buf;
};
/*!
 Storage object for float data
*/
class storeFloat : public storeBase {
 public:
  //! Create an float storage buffer
  /*!
    \param n Size of the buffer
  */
  storeFloat(const size_t n) {
    cleanMemory();
    _buf = new float[n];
  }
  //! Create an float storage buffer
  /*!
    \param n Size of the buffer
    \param buf Data to store in buffer
  */
  storeFloat(const size_t n, void *buf);
  //! Copy the contents of buffer into buf
  /*!
    \param buf Data to store contents of  buffer into
  */
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  //! Return a pointer to buffer

  char *getPtr() override { return (char *)_buf.data(); }
  //! Put data into class from buf
  /*!
    \param buf  Base class
  */
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  //! Zero a buffer

  virtual void zero() override { cleanMemory(); }
  //! Clone a buffer

  virtual std::shared_ptr<storeBase> clone() const override;
  //! Return info about buffer

  virtual void info(const std::string &v) const override;
  //! Get window from global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG,
                         const void *bufIn) override;
  //! Get size of a single element

  virtual size_t getElementSize() const override { return sizeof(float); }
  //! Get the size of the dataset

  virtual size_t getSize() const override { return _buf.size(); }

  //! Clean memory
  inline void cleanMemory() {
    if (_buf) {
      delete[] _buf;
      _buf = NULL;
    }
  }

  //! Destructor
  virtual ~storeFloat() { cleanMemory(); }

 private:
  float *_buf;
};
/*!
 Storage object for double data
*/
class storeDouble : public storeBase {
 public:
  //! Create an double storage buffer
  /*!
    \param n Size of the buffer
  */
  storeDouble(const size_t n) {
    cleanMemory();
    _buf = new double[n];
  }
  //! Create an double storage buffer
  /*!
    \param n Size of the buffer
    \param buf Data to store in buffer
  */
  storeDouble(const size_t n, void *buf);
  //! Copy the contents of buffer into buf
  /*!
    \param buf Data to store contents of  buffer into
  */
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  //! Return a pointer to buffer

  char *getPtr() override { return (char *)_buf.data(); }
  //! Put data into class from buf
  /*!
    \param buf  Base class
  */
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  //! Zero a buffer

  virtual void zero() override { cleanMemory(); }
  //! Clone a buffer

  virtual std::shared_ptr<storeBase> clone() const override;
  //! Get window from global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG,
                         const void *bufIn) override;
  //! Get size of a single element

  virtual size_t getElementSize() const override { return sizeof(double); }
  //! Get the size of the dataset

  virtual size_t getSize() const override { return _buf.size(); }

  inline void cleanMemory() {
    if (_buf) {
      delete[] _buf;
      _buf = NULL;
    }
  }

  //! Destructor
  virtual ~storeDouble() { cleanMemory(); }

 private:
  double *_buf;
};
/*!
 Storage object for complex float data
*/
class storeComplex : public storeBase {
 public:
  //! Create an complex float storage buffer
  /*!
    \param n Size of the buffer
  */
  storeComplex(const size_t n) { _buf = new std::complex<float>[n]; }
  //! Create an complex storage buffer
  /*!
    \param n Size of the buffer
    \param buf Data to store in buffer
  */
  storeComplex(const size_t n, void *buf);
  //! Copy the contents of buffer into buf
  /*!
    \param buf Data to store contents of  buffer into
  */
  virtual void getData(std::shared_ptr<storeBase> buf) const override;
  //! Return a pointer to buffer

  char *getPtr() override { return (char *)_buf.data(); }
  //! Put data into class from buf
  /*!
    \param buf  Base class
  */
  virtual void putData(const std::shared_ptr<storeBase> buf) override;
  //! Zero a buffer

  virtual void zero() override { cleanMemory(); }
  //! Clone a buffer

  virtual std::shared_ptr<storeBase> clone() const override;
  //! Get window from global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void getWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG, void *bufIn) override;
  //! Put window to global
  /*!
    \param nwL Number of samples in buf
    \param fwL First sample along each axis in buf
    \param jwL Jump between samples along each axis in buf
    \param nbL Blocking (1,n[0],n[1]*n[0],etc
    \param fwG First sample along each axis in Global
    \param nbG Blocking of global buffer
    \param bufIn Buffer
  */
  virtual void putWindow(const std::vector<int> &nwL,
                         const std::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nbL,
                         const std::vector<int> &fwG,
                         const std::vector<int> &nbG,
                         const void *bufIn) override;
  //! Get size of a single element

  virtual size_t getElementSize() const override {
    return sizeof(std::complex<float>);
  }
  //! Get the size of the dataset

  virtual size_t getSize() const override { return _buf.size(); }

  inline void cleanMemory() {
    if (_buf) {
      delete[] _buf;
      _buf = NULL;
    }
  }

  //! Destructor
  virtual ~storeComplex() { cleanMemory(); }

 private:
  std::complex<float> *_buf;
};
//!  Return the storage type of a buffer
/*!
  \param bufIn Input buffer
*/
std::string returnStorageType(const std::shared_ptr<storeBase> bufIn);
//!  Return a storage of given buffer type
/*!
  \param typ  Type of storage
  \param n Number of elements

*/
std::shared_ptr<storeBase> returnStorage(const dataType typ, const size_t n);
//! Throw an error due to data type mismach
/*!
  \param myType Type of buffer
  \param typeSent Type of buffer passed
*/
void throwError(const std::string myType, const std::string typeSent);

}  // namespace IO
}  // namespace SEP
#endif
