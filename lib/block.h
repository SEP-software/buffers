#ifndef BLOCK_H
#define BLOCK_H 1
#include <cmath>
#include <memory>
#include <utility>
#include "SEPException.h"
#include "hypercube.h"
namespace SEP {
  namespace blocking {

  /*!
  Class that holds a subset of a dataset*/
  class basicSubset {
   public:
    basicSubset() { ; }
    basicSubset(const std::shared_ptr<SEP::hypercube> hyper) {
      fromHyper(hyper);
    }
    /*! Create a datasubset representing entire hypercube
     */
    void fromHyper(const std::shared_ptr<SEP::hypercube> hyper) {
      _nG = std::vector<int>(7, 1);
      _nW = std::vector<int>(7, 1);
      _fW = std::vector<int>(7, 0);
      _jW = std::vector<int>(7, 1);
      _bG = std::vector<long long>(8, 1);

      std::vector<int> ns = hyper->getNs();
      for (auto i = 0; i < ns.size(); i++) {
        _nW[i] = _nG[i] = ns[i];
        _bG[i + 1] = _bG[i] * ns[i];
      }
      for (int i = ns.size(); i < 7; i++) {
        _bG[i + 1] = _bG[i];
      }
    }
    /*! Basic setup of subset*/
    void basicSetup(const std::vector<int> nG, const std::vector<int> nW,
                    const std::vector<int> fW, const std::vector<int> jW,
                    bool resetFJ = false) {
      _nG = nG;
      _fW = fW;
      _jW = jW;
      _nW = nW;
      _bG.push_back(1);
      for (int i = 0; i < _nG.size(); i++) {
        _bG.push_back(_bG[i] * (long long)_nG[i]);
        if (resetFJ) {
          _fW[i] = 0;
          _jW[i] = 1;
        }
      }
      _n123 = _bG[_nG.size()];
      for (int i = _nG.size(); i < 7; i++) {
        _nG.push_back(1);
        _nW.push_back(1);
        _fW.push_back(0);
        _jW.push_back(1);
        _bG.push_back(_bG[i]);
      }
    }

    std::vector<int> _nG, _fW, _jW, _nW;  ///! Window params for this subset
    std::vector<long long> _bG;           ///< 1,n1,n1*n2,...
    long long _n123;                      ///< Number of elements in the dataset
  };

  /*!
   Class describing a subset of a hypercube*/
  template <typename T>
  class dataSubset : public basicSubset {
   public:
    /*! Create a datasubset representing entire hypercube
     */
    dataSubset(std::shared_ptr<SEP::hypercube> hyper) { fromHyper(hyper); }
    /*!
    Initialize datga subset without storage
  \param nG  Full dimension of the hypercube
  \param nW,fW,jW Window of this subset
  */

    dataSubset(const std::vector<int> nG, const std::vector<int> nW,
               const std::vector<int> fW, const std::vector<int> jW) {
      basicSetup(nG, nW, fW, jW);
    }

    /*!
    Initialize datga subset with storage
  \param nG  Full dimension of the hypercube
  \param nW,fW,jW Window of this subset
  \param dat Pointer to the data
  \param resetFJ Whther or not reset j to 1 and f to 0

  */
    dataSubset(const std::vector<int> nG, const std::vector<int> nW,
               const std::vector<int> fW, const std::vector<int> jW, T *dat,
               bool resetFJ = false)
        : _dat{dat} {
      basicSetup(nG, nW, fW, jW, resetFJ);
    }
    /*!
     Make a clone of the current subset and set data pointer
     \param dat Pointer to the storage
     \oaram resetFJ Wthether or not to set f and j window params for the subset
     */
    std::shared_ptr<dataSubset<T>> cloneSetData(T *dat, bool resetFJ = false) {
      std::shared_ptr<dataSubset<T>> c(
          new dataSubset<T>(_nG, _nW, _fW, _jW, dat, resetFJ));
      return c;
    }
    /*!
    Set the storage for the subset
    \param dat Storagage
    */
    void setData(T *dat) { _dat = dat; }
    /*  Set no storage associated with object*/
    void nullStorage() { _dat = nullptr; }

    T *_dat;  ///< data storage
  };

  /*!
   Class for moving data between subsets*/
  template <typename T>
  class block {
   public:
    /*!
    Default initialzier*/
    block() { ; }
    /*! Initalize with a description of the subset
    \param bl Description of subset for block
    */
    block(const std::shared_ptr<dataSubset<T>> bl) : _sub{bl} {}
    /*
      Copy into this block

      \param fromT Description of the subset we are copying from
      \param fromD Storage from
      \param toD Storage to
      */
    void toBlock(const std::shared_ptr<dataSubset<T>> fromT, T *fromD, T *toD) {
      std::shared_ptr<dataSubset<T>> to = _sub->cloneSetData(toD, true),
                                     from = fromT->cloneSetData(fromD);

      moveData(from, to);
    }
    /*
    Copy out of this block

    \param fromT Description of the subset we are copying from
    \param fromD Storage from
    \param toD Storage to
    */
    void fromBlock(std::shared_ptr<dataSubset<T>> toT, T *fromD, T *toD) {
      std::shared_ptr<dataSubset<T>> to = toT->cloneSetData(toD),
                                     from = _sub->cloneSetData(fromD, true);
      moveData(from, to);
    }

    /*!
     \param wind Window parameters
     \return pair(what the winD->w represents in global coordinates, local
     coordinates)
     */

    std::pair<std::shared_ptr<dataSubset<T>>, std::shared_ptr<dataSubset<T>>>
    localWindow(std::shared_ptr<dataSubset<T>> wind) {
      return localWindow(wind->_nG, wind->_nW, wind->_fW, wind->_jW);
    }
    /*!
     \param nw Number of samples each axis needed for global operation
     \param fw Origin along  each axis needed for global operation
     \param jw Skip of element along each axis for global operation
     \return pair(what the winD->w represents in global coordinates, local
     coordinates)

    */
    std::pair<std::shared_ptr<dataSubset<T>>, std::shared_ptr<dataSubset<T>>>
    localWindow(const std::vector<int> &nG, const std::vector<int> &nw,
                const std::vector<int> &fw, const std::vector<int> &jw) {
      std::vector<int> nwG(7), fwG(7), jwG(7), nwL(7), fwL(7), jwL(7);
      size_t i = 0;
      for (i = 0; i < nwL.size(); i++) {
        int nbeforeBuffer = 0;
        // First sample
        if (fw[i] > _sub->_fW[i]) {  // The winD->w starts after the buffer
          fwL[i] = ceilf(float(fw[i] - _sub->_fW[i]) / jw[i]) * jw[i];
        } else {  // We just need to figure what sample we start at

          fwL[i] = ceilf(float(_sub->_fW[i] - fw[i]) / float(jw[i])) * jw[i] +
                   fw[i] - _sub->_fW[i];
          nbeforeBuffer = int(float(_sub->_fW[i] - fw[i]) / float(jw[i]));
        }

        assert(nbeforeBuffer < nw[i]);

        nwL[i] =
            std::min((int)(ceilf(float(_sub->_nW[i] - fwL[i]) / float(jw[i]))),
                     nw[i] - nbeforeBuffer);
        // Is the first sample outside this patch?
        assert(fwL[i] < _sub->_nw[i]);
        // if (f_w[i] > _n[i]) return 0;

        assert(fwL[i] >= 0);
        // subtract off the points already used in previous cells
        // n_w[i] = nw[i] - nusedBuf;

        // If less 0 we are done
        assert(nwL[i] > 0);
        //    if (n_w[i] < 1) return 0;
        jwL[i] = jw[i];
        jwG[i] = jw[i];

        if (_sub->_fW[i] < fw[i]) {
          fwG[i] = 0;
        } else {
          fwG[i] = ceilf(float((_sub->_fW[i] - fw[i])) / float(jw[i]));
        }
        assert(fwG[i] >= 0);
        assert(fwG[i] < nw[i]);
      }

      std::shared_ptr<dataSubset<T>> b(new dataSubset<T>(nG, nwG, fwG, jwG)),
          s(new dataSubset<T>(nwL, nwL, fwL, jwL));
      return std::pair<std::shared_ptr<dataSubset<T>>,
                       std::shared_ptr<dataSubset<T>>>(b, s);
    }

   private:
    std::shared_ptr<dataSubset<T>> _sub;
  };
  /*!
   MOve data between two subset */
  template <typename T>
  void moveData(const std::shared_ptr<dataSubset<T>> inD,
                std::shared_ptr<dataSubset<T>> outD) {
    if (inD->_dat == nullptr || outD->_dat == nullptr)
      throw SEPException("input and output subset data must be set");
    const T *inB = inD->_dat;
    T *outB = outD->_dat;

    if (inD->_jW[0] == outD->_jW[0] && inD->_jW[0] == 1 && 3 == 4) {
      for (int i6L = 0; i6L < outD->_nW[6]; i6L++) {
        size_t f6L = outD->_bG[6] * (outD->_fW[6] + i6L * outD->_jW[6]);
        size_t f6G = inD->_bG[6] * (inD->_fW[6] + i6L * inD->_jW[6]);
        for (int i5L = 0; i5L < outD->_nW[5]; i5L++) {
          size_t f5L = f6L + outD->_bG[5] * (outD->_fW[5] + i5L * outD->_jW[5]);
          size_t f5G = inD->_bG[5] * (inD->_fW[5] + i5L * inD->_jW[4]) + f6G;
          for (int i4L = 0; i4L < outD->_nW[4]; i4L++) {
            size_t f4L =
                f5L + outD->_bG[4] * (outD->_fW[4] + outD->_jW[4] * i4L);
            size_t f4G = inD->_bG[4] * (inD->_fW[4] + i4L * inD->_jW[4]) + f5G;
            for (int i3L = 0; i3L < outD->_nW[3]; i3L++) {
              size_t f3L =
                  f4L + outD->_bG[3] * (outD->_fW[3] + outD->_jW[3] * i3L);
              size_t f3G =
                  inD->_bG[3] * (inD->_fW[3] + i3L * inD->_jW[3]) + f4G;
              for (int i2L = 0; i2L < outD->_nW[2]; i2L++) {
                size_t f2L =
                    f3L + outD->_bG[2] * (outD->_fW[2] + outD->_jW[2] * i2L);
                size_t f2G =
                    inD->_bG[2] * (inD->_fW[2] + i2L * inD->_jW[2]) + f3G;

                for (int i1L = 0; i1L < outD->_nW[1]; i1L++) {
                  size_t f1L =
                      f2L + outD->_bG[1] * (outD->_fW[1] + outD->_jW[1] * i1L) +
                      outD->_fW[0];
                  size_t f1G = inD->_bG[1] * (inD->_fW[1] + i1L * inD->_jW[1]) +
                               f2G + inD->_fW[0];

                  memcpy(outB + f1L, inB + f1G, sizeof(T) * outD->_nW[0]);
                }
              }
            }
          }
        }
      }

    } else {
      for (int i6L = 0; i6L < outD->_nW[6]; i6L++) {
        size_t f6L = outD->_bG[6] * (outD->_fW[6] + i6L * outD->_jW[6]);
        size_t f6G = inD->_bG[6] * (inD->_fW[6] + i6L * inD->_jW[6]);
        for (int i5L = 0; i5L < outD->_nW[5]; i5L++) {
          size_t f5L = f6L + outD->_bG[5] * (outD->_fW[5] + i5L * outD->_jW[5]);
          size_t f5G = inD->_bG[5] * (inD->_fW[5] + i5L * inD->_jW[4]) + f6G;
          for (int i4L = 0; i4L < outD->_nW[4]; i4L++) {
            size_t f4L =
                f5L + outD->_bG[4] * (outD->_fW[4] + outD->_jW[4] * i4L);
            size_t f4G = inD->_bG[4] * (inD->_fW[4] + i4L * inD->_jW[4]) + f5G;
            for (int i3L = 0; i3L < outD->_nW[3]; i3L++) {
              size_t f3L =
                  f4L + outD->_bG[3] * (outD->_fW[3] + outD->_jW[3] * i3L);
              size_t f3G =
                  inD->_bG[3] * (inD->_fW[3] + i3L * inD->_jW[3]) + f4G;
              for (int i2L = 0; i2L < outD->_nW[2]; i2L++) {
                size_t f2L =
                    f3L + outD->_bG[2] * (outD->_fW[2] + outD->_jW[2] * i2L);
                size_t f2G =
                    inD->_bG[2] * (inD->_fW[2] + i2L * inD->_jW[2]) + f3G;
                for (int i1L = 0; i1L < outD->_nW[1]; i1L++) {
                  size_t f1L =
                      f2L + outD->_bG[1] * (outD->_fW[1] + outD->_jW[1] * i1L) +
                      outD->_fW[0];
                  size_t f1G = inD->_bG[1] * (inD->_fW[1] + i1L * inD->_jW[1]) +
                               f2G + inD->_fW[0];
                  for (size_t i0L = 0; i0L < outD->_nW[0]; i0L++) {
                    outB[i0L + f1L * outD->_jW[0]] =
                        inB[f1G + i0L * inD->_jW[0]];
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  }  // namespace blocking
}  // namespace SEP

#endif
