#include "memoryAll.h"
#include <iostream>
using namespace SEP::IO;
void memoryAll::updateRecentBuffers(const std::vector<int> &bufs) {
  if (bufs.size() == 0)
    ;
  return;
}
std::shared_ptr<memoryReduce> memoryAll::changeBufferState(
    const long memChange) {
  if (memChange == 0)
    ;
  std::vector<int> a, b;
  std::shared_ptr<memoryReduce> x(new memoryReduce(a, b));
  return x;
}
