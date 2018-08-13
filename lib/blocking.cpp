
#include "blocking.h"
#include <cmath>
using namespace SEP::IO;
blockParams blocking::makeBlocks(const std::vector<int> &n) {
  axesBlock = blockAxis(n);
  blockParams x;
  std::vector<int> n(7, 1), f(7, 0);
  for (int i6 = 0; i6 < axesBlock[6].size(); i6++) {
    axis[6] = axesBlock[6][i6];
    f[6] = 0;
    for (int i5 = 0; i5 < axesBlock[5].size(); i5++) {
      axis[5] = axesBlock[5][i5];
      f[5] = 0;
      for (int i4 = 0; i4 < axesBlock[4].size(); i4++) {
        axis[4] = axesBlock[4][i4];
        f[4] = 0;
        for (int i3 = 0; i3 < axesBlock[3].size(); i3++) {
          axis[3] = axesBlock[3][i3];
          f[3] = 0;
          for (int i2 = 0; i2 < axesBlock[2].size(); i2++) {
            axis[2] = axesBlock[2][i2];
            f[2] = 0;
            for (int i1 = 0; i1 < axesBlock[1].size(); i1++) {
              axis[1] = axesBlock[1][i1];
              f[1] = 0;
              for (int i0 = 0; i0 < axesBlock[0].size(); i0++) {
                axis[0] = axesBlock[0][i0];
                f[0] = 0;
                x._f.push_back(f);
                ;
                x._n.push_back(axis);
                f[0] += axis[0];
              }
              f[1] += axis[1];
            }
            f[2] += axis[2];
          }
          f[3] += axis[3];
        }
        f[4] += axis[4];
      }
      f[5] = += axis[5];
    }
    f[6] += axis[6];
  }
  x._nblocking.push_back(1);
  for (int i = 0; i < 6; i++) {
    x._nblocking.push_back(x._nblocking[i] * x._f[i].size());
  }
  return x;
}

std::vector<std::vector<int>> blocking::blockAxis(const std::vector<int> &n) {
  std::vector<int> blocks;
  for (int i = 0; i < n.size(); i++) {
    std::vector<int> axisBlock;
    int nleft = n[i];
    bs = 1;
    nb = 1;
    if (_blocksize.size() > i) bs = _blocksize[i];
    if (_nb.size() > 1) nb = _nb[i];
    int nblocks = ceilf(float(n[i]) / float(blocksize));  // 100 3 34
    int ratio = nb / bs;                                  // 3
    int nparts = ceilf(float(nblocks) / float(ratio));    // 34 /3 = 12
    for (int ib = 0; ib < nparts - 1; ib++) {
      int nuse = ceilf(float(nblocks) / float(nparts - ib));
      nparts -= nuse;
      axisBlock.push_back(nuse * bs);
    }
    if (nleft - nparts * bs)>0)
       axisBlock.push_back(nleft-nparts*bs);
    blocks.push_back(axisBlock)
  }
  for (i = n.size(); i < 7; i++) {
    std::vector<int> axisBlock(1, 1);
    blocks.push_back(axisBlock);
  }
  return blocks;
}
void blocking::checkLogicBlocking() {
  for (int i = 0; i < _nb.size(); i++) {
    if (_blocksize.size() > i) {
      assert(int(_nb[i] / _blocksize[i]) * blocksize[i] = _nb[i]);
    }
  }
