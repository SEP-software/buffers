
#include "blocking.h"
#include <cmath>
#include <iostream>
#include "SEPException.h"
using namespace SEP::IO;
blockParams blocking::makeBlocks(const std::vector<int> &nsz) {
  blockParams x;
  x._axesBlock = blockAxis(nsz);

  std::vector<int> n(7, 1), f(7, 0), axis(7, 0);
  f[6] = 0;
  for (int i6 = 0; i6 < x._axesBlock[6].size(); i6++) {
    axis[6] = x._axesBlock[6][i6];
    f[5] = 0;
    for (int i5 = 0; i5 < x._axesBlock[5].size(); i5++) {
      axis[5] = x._axesBlock[5][i5];
      f[4] = 0;
      for (int i4 = 0; i4 < x._axesBlock[4].size(); i4++) {
        axis[4] = x._axesBlock[4][i4];
        f[3] = 0;
        for (int i3 = 0; i3 < x._axesBlock[3].size(); i3++) {
          axis[3] = x._axesBlock[3][i3];
          f[2] = 0;
          for (int i2 = 0; i2 < x._axesBlock[2].size(); i2++) {
            axis[2] = x._axesBlock[2][i2];
            f[1] = 0;
            for (int i1 = 0; i1 < x._axesBlock[1].size(); i1++) {
              axis[1] = x._axesBlock[1][i1];
              f[0] = 0;
              for (int i0 = 0; i0 < x._axesBlock[0].size(); i0++) {
                axis[0] = x._axesBlock[0][i0];
                x._fs.push_back(f);
                x._ns.push_back(axis);
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
      f[5] += axis[5];
    }
    f[6] += axis[6];
  }
  x._nblocking.push_back(1);
  for (int i = 0; i < 6; i++) {
    x._nblocking.push_back(x._nblocking[i] * x._axesBlock[i].size());
  }
  return x;
}

blocking::blocking(const Json::Value &jsonArgs) {
  if (jsonArgs["blocksize"].isNull())
    throw SEPException(std::string("trouble grabing blocksize"));

  for (auto itr : jsonArgs["blocksize"]) _blocksize.push_back(itr.asInt());

  if (jsonArgs["nb"].isNull())
    throw SEPException(std::string("trouble grabing nb"));

  for (auto itr : jsonArgs["nb"]) _nb.push_back(itr.asInt());

  checkLogicBlocking();
}
std::shared_ptr<blocking> blocking::createDefaultBlocking(
    std::shared_ptr<SEP::hypercube> hyper) {
  std::vector<int> bs(4, 1), block(4, 1);
  if (hyper->getAxis(1).n > 64)
    bs[0] = 1;
  else
    bs[0] = 64;

  if (hyper->getNdimG1() == 1) {
    block[0] = 256 * 1024;
  } else if (hyper->getNdimG1() == 2) {
    bs[1] = std::min(hyper->getAxis(2).n, 4);
    block[0] = bs[0] * 8;
    block[1] = bs[1] * 256;
  } else if (hyper->getNdimG1() == 3) {
    bs[1] = std::min(hyper->getAxis(2).n, 4);
    bs[2] = std::min(hyper->getAxis(3).n, 4);
    block[0] = bs[0] * 8;
    block[1] = bs[1] * 8;
    block[2] = bs[2] * 8;

  } else {
    bs[1] = std::min(hyper->getAxis(2).n, 4);
    bs[2] = std::min(hyper->getAxis(3).n, 4);
    bs[3] = std::min(hyper->getAxis(4).n, 4);
    block[0] = bs[0] * 8;
    block[1] = bs[1] * 8;
    block[2] = bs[2] * 8;
    block[3] = bs[3] * 8;
  }

  std::shared_ptr<blocking> b(new blocking(bs, block));

  return b;
}
Json::Value blocking::getJsonDescription() {
  Json::Value v;
  Json::Value vals;
  if (_blocksize.size() == 0)
    throw SEPException(
        std::string("Blocking not defined can't return description"));
  for (auto i = 0; i < _blocksize.size(); i++) vals.append(_blocksize[i]);
  v["blocksize"] = vals;

  Json::Value val2;
  for (auto i = 0; i < _nb.size(); i++) val2.append(_nb[i]);
  v["nb"] = val2;

  return v;
}
std::vector<std::vector<int>> blocking::blockAxis(const std::vector<int> &n) {
  std::vector<std::vector<int>> blocks;
  for (int i = 0; i < n.size(); i++) {
    std::vector<int> axisBlock;
    int nleft = n[i];
    int bs = 1;
    int nb = 1;
    if (_blocksize.size() > i) bs = _blocksize[i];

    if (_nb.size() > i) nb = _nb[i];
    //
    int nblocks = ceilf(float(n[i]) / float(_blocksize[i]));  // 100 3 34
    int ratio = nb / bs;                                      // 3
    int nparts = ceilf(float(nblocks) / float(ratio));        // 34 /3 = 12
    std::cerr << nblocks << "=nblocks n=" << n[i]
              << " blocksize=" << _blocksize[i] << " nb=" << nb << " bs=" << bs
              << " nparts=" << nparts << std::endl;
    for (int ib = 0; ib < nparts - 1; ib++) {
      int nuse = ceilf(float(nblocks) / float(nparts - ib));
      nblocks -= nuse;
      nleft -= nuse * bs;

      axisBlock.push_back(nuse * bs);
    }

    if (nleft > 0) axisBlock.push_back(nleft);
    std::cerr << "iax=" << i << " " << axisBlock.size() << " " << axisBlock[0]
              << std::endl;
    blocks.push_back(axisBlock);
  }

  for (size_t i = n.size(); i < 7; i++) {
    std::vector<int> axisBlock(1, 1);
    blocks.push_back(axisBlock);
  }
  throw SEPException("error");
  return blocks;
}
void blocking::checkLogicBlocking() {
  for (int i = 0; i < _nb.size(); i++) {
    if (_blocksize.size() > i) {
      if (int(_nb[i] / _blocksize[i]) * _blocksize[i] != _nb[i]) {
        throw SEPException(std::string("axis ") + std::to_string(i) +
                           std::string(" blockElement=") +
                           std::to_string(_nb[i]) + std::string(" blockSize=") +
                           std::to_string(_blocksize[i]) + " not divisible");
      }
    }
  }
}
