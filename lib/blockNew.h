#ifndef BLOCK_NEW_H
#define BLOCK_NEW_H 1
namesapce SEP {
  namespace buffers {

  template <typename T>
  class blockP {
   public
    ::blockP(const std::vector<int> nG, const std::vector<int> fW,
             const std::vector<int> jW, const std::vector<int> nW, T *dat)
        : _nG(nG), _fW(fW), _jW(jW), _nW(nW), _dat(dat);
    {}
    std::vector<int> _nG, _fW, _jW, _nW;
    T *_dat;
  };

  template <typename T>
  class blockNew {
    blockNew() { ; }
    virtual void calcBlocks() {
      throw SEPException("Must overrride calcBlocks");
    }

    void moveData(const blockP<T> &inD, blockP<T> &outD) {
      const T *inB = inD._dat;
      T *outT = outB._dat;
      if (inD._jW[0] == outD._jW[0] && inD._jW[0] == 1) {
        for (int i6L = 0; i6L < outD._nW[6]; i6L++) {
          size_t f6L = outD._nG[6] * (outD._fW[6] + i6L * outD._jW[6]);
          size_t f6G = inD._nG[6] * (inD._fW[6] + i6L * inD._jW[6]);
          for (int i5L = 0; i5L < outD._nW[5]; i5L++) {
            size_t f5L = f6L + outD._nG[5] * (outD._fW[5] + i5L * outD._jW[5]);
            size_t f5G = inD._nG[5] * (inD._fW[5] + i5L * inD._jW[4]) + f6G;
            for (int i4L = 0; i4L < outD._nW[4]; i4L++) {
              size_t f4L =
                  f5L + outD._nG[4] * (outD._fW[4] + outD._jW[4] * i4L);
              size_t f4G = inD._nG[4] * (inD._fW[4] + i4L * inD._jW[4]) + f5G;
              for (int i3L = 0; i3L < outD._nW[3]; i3L++) {
                size_t f3L =
                    f4L + outD._nG[3] * (outD._fW[3] + outD._jW[3] * i3L);
                size_t f3G = inD._nG[3] * (inD._fW[3] + i3L * inD._jW[3]) + f4G;
                for (int i2L = 0; i2L < outD._nW[2]; i2L++) {
                  size_t f2L =
                      f3L + outD._nG[2] * (outD._fW[2] + outD._jW[2] * i2L);
                  size_t f2G =
                      inD._nG[2] * (inD._fW[2] + i2L * inD._jW[2]) + f3G;
                  for (int i1L = 0; i1L < outD._nW[1]; i1L++) {
                    size_t f1L =
                        f2L + outD._nG[1] * (outD._fW[1] + outD._jW[1] * i1L) +
                        outD._fW[0];
                    size_t f1G = inD._nG[1] * (inD._fW[1] + i1L * inD._jW[1]) +
                                 f2G + inD._fW[0];
                    memcpy(outB + f1L, inB + f1G, sizeof(T) * outD._nw[0]);
                  }
                }
              }
            }
          }
        }

      } else {
        for (int i6L = 0; i6L < outD._nW[6]; i6L++) {
          size_t f6L = outD._nG[6] * (outD._fW[6] + i6L * outD._jW[6]);
          size_t f6G = inD._nG[6] * (inD._fW[6] + i6L * inD._jW[6]);
          for (int i5L = 0; i5L < outD._nW[5]; i5L++) {
            size_t f5L = f6L + outD._nG[5] * (outD._fW[5] + i5L * outD._jW[5]);
            size_t f5G = inD._nG[5] * (inD._fW[5] + i5L * inD._jW[4]) + f6G;
            for (int i4L = 0; i4L < outD._nW[4]; i4L++) {
              size_t f4L =
                  f5L + outD._nG[4] * (outD._fW[4] + outD._jW[4] * i4L);
              size_t f4G = inD._nG[4] * (inD._fW[4] + i4L * inD._jW[4]) + f5G;
              for (int i3L = 0; i3L < outD._nW[3]; i3L++) {
                size_t f3L =
                    f4L + outD._nG[3] * (outD._fW[3] + outD._jW[3] * i3L);
                size_t f3G = inD._nG[3] * (inD._fW[3] + i3L * inD._jW[3]) + f4G;
                for (int i2L = 0; i2L < outD._nW[2]; i2L++) {
                  size_t f2L =
                      f3L + outD._nG[2] * (outD._fW[2] + outD._jW[2] * i2L);
                  size_t f2G =
                      inD._nG[2] * (inD._fW[2] + i2L * inD._jW[2]) + f3G;
                  for (int i1L = 0; i1L < outD._nW[1]; i1L++) {
                    size_t f1L =
                        f2L + outD._nG[1] * (outD._fW[1] + outD._jW[1] * i1L) +
                        outD._fW[0];
                    size_t f1G = inD._nG[1] * (inD._fW[1] + i1L * inD._jW[1]) +
                                 f2G + inD._fW[0];
                    for (size_t i0L = 0; i0L < outD._nW[0]; i0L++) {
                      outB[i0L + f1G * inD._jW[0]] =
                          inB[f1L + i0L * outD._jW[0]];
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  };
  }  // namespace buffers
}
#endif
