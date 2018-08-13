#ifndef STORE_H
#define STORE_H 1
#include <complex>
#include <memory>
#include <sstream>
#include <vector>
namespace SEP {
namespace IO {
class storeBase {
 public:
  virtual void getData(std::shared_ptr<storeBase> buf) const = 0;
  virtual void putData(const std::shared_ptr<storeBase> buf);
  virtual void getWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                         const std::vector<int> &jw, const std::vector<int> &nb,
                         std::shared_ptr<storeBase> bufIn, const int ibeg);
  virtual void putWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                         const std::vector<int> &jw, const std::vector<int> &nb,
                         std::shared_ptr<storeBase> bufIn, const int ibeg);
  virtual std::shared_ptr<storeBase> clone() const = 0;
  virtual int getSize() const = 0;
  virtual char *getPtr();
};

template <class V>
class store : public storeBase {
 public:
  store(const int n) { _buf.resize(n); }
  void getData(std::shared_ptr<storeBase> buf) const {
    std::shared_ptr<store<V>> b = std::dynamic_pointer_cast<store<V>>(buf);
    assert(b);
    assert(b->_buf.size() <= _buf.size());
    memcpy(b->_buf.data(), _buf.data(), _buf.size() * sizeof(V));
  }
  char *getPtr() { return (char *)_buf.data(); }
  void putData(const std::shared_ptr<store> buf) {
    const std::shared_ptr<store<V>> b =
        std::dynamic_pointer_cast<store<V>>(buf);
    assert(b);
    assert(b->_buf.size() == _buf.size());
    memcpy(_buf.data(), b->_buf.data(), _buf.size() * sizeof(V));
  }

  virtual std::shared_ptr<storeBase> clone() const {
    std::shared_ptr<store<V>> m(new store<V>((int)_buf.size()));
    memcpy(m->_buf.data(), _buf.data(), _buf.size() * sizeof(V));

    return m;
  }

  virtual void getWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                         const std::vector<int> &jw, const std::vector<int> &nb,
                         std::shared_ptr<storeBase> bufIn, const int ibeg) {
    std::shared_ptr<store<V>> in = std::dynamic_pointer_cast<store<V>>(bufIn);

    int ipos = ibeg;
    for (int i6 = 0; i6 < nw[6]; i6++) {
      int f6 = nb[6] * (fw[6] + i6 * jw[6]);
      for (int i5 = 0; i5 < nw[5]; i5++) {
        int f5 = f6 + nb[5] * (fw[5] + i5 * jw[5]);
        for (int i4 = 0; i4 < nw[4]; i4++) {
          int f4 = f5 + nb[4] * (fw[4] + jw[4] * i4);
          for (int i3 = 0; i3 < nw[3]; i3++) {
            int f3 = f4 + nb[3] * (fw[3] + jw[3] * i3);
            for (int i2 = 0; i2 < nw[2]; i2++) {
              int f2 = f3 + nb[2] * (fw[2] + jw[2] * i2);
              for (int i1 = 0; i1 < nw[1]; i1++) {
                int f1 = f2 + nb[1] * (fw[1] + jw[1] * i1) + fw[0];
                for (int i0 = 0; i0 < nw[0]; i0++) {
                  _buf[ipos++] = in->_buf[f1 + i0 * jw[0]];
                }
              }
            }
          }
        }
      }
    }
  }
  virtual void putWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                         const std::vector<int> &jw, const std::vector<int> &nb,
                         std::shared_ptr<storeBase> bufIn, const int ibeg) {
    const std::shared_ptr<store<V>> in =
        std::dynamic_pointer_cast<store<V>>(bufIn);
    int ipos = ibeg;
    for (int i6 = 0; i6 < nw[6]; i6++) {
      int f6 = nb[6] * (fw[6] + i6 * jw[6]);
      for (int i5 = 0; i5 < nw[5]; i5++) {
        int f5 = f6 + nb[5] * (fw[5] + i5 * jw[5]);
        for (int i4 = 0; i4 < nw[4]; i4++) {
          int f4 = f5 + nb[4] * (fw[4] + jw[4] * i4);
          for (int i3 = 0; i3 < nw[3]; i3++) {
            int f3 = f4 + nb[3] * (fw[3] + jw[3] * i3);
            for (int i2 = 0; i2 < nw[2]; i2++) {
              int f2 = f3 + nb[2] * (fw[2] + jw[2] * i2);
              for (int i1 = 0; i1 < nw[1]; i1++) {
                int f1 = f2 + nb[1] * (fw[1] + jw[1] * i1) + fw[0];
                for (int i0 = 0; i0 < nw[0]; i0++) {
                  _buf[f1 + i0 * jw[0]] = in->_buf[ipos++];
                }
              }
            }
          }
        }
      }
    }
  }

  virtual int getSize() const { return sizeof(V); }

 private:
  std::vector<V> _buf;
};

}  // namespace IO
}  // namespace SEP
#endif