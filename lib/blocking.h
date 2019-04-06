#ifndef BLOCKING_H
#define BLOCKING_H 1
#include <json.h>
#include <memory>
#include <vector>
#include "hypercube.h"
namespace SEP {
namespace IO {
/*!
 Parameters for blocking a regular sampled function
*/

class blockParams {
 public:
  blockParams() { ; }
  std::vector<std::vector<int>> _fs;  ///< A vector of all blocks, describing
                                      ///< the begining position along each axis
  std::vector<std::vector<int>>
      _ns;  ///< A vector of all blocks, describing the size along each axis
  std::vector<int>
      _nblocking;  ///< 1,
                   ///< len(_axesBlock[0]),len(_axesBlock[1])*len(_axesBlock[0]),..
  std::vector<std::vector<int>>
      _axesBlock;  ///< A vectror of how each axis is blocked
};
/*!
  Class describing how to break a hypercube into sub-cubes
*/
class blocking {
 public:
  //!  Create a blocking object
  /*!
    \param  blocksize  Minimum unit to try to break axes into
    \param nb  Desired size for each axis
  */
  blocking(const std::vector<int> &blocksize, std::vector<int> &nb) {
    _blocksize = blocksize;
    _nb = nb;
    checkLogicBlocking();
  }
  //!  Create a blocking object
  /*!
    \param  jsonArgs  JSON object describing blocking
  */
  blocking(const Json::Value &jsonArgs);
  //! Return how to block a given dataset
  /*!
    \param n Size of dataset
  */
  blockParams makeBlocks(const std::vector<int> &n);
  //! Check to see if blocking parameters make sense
  void checkLogicBlocking();
  //!  How to block an axis
  /*!
    \param  n Size
  */
  std::vector<std::vector<int>> blockAxis(const std::vector<int> &n);
  //! Return JSON description of the current blocking
  Json::Value getJsonDescription();
  //! Create the defaukt blocking object
  static std::shared_ptr<blocking> createDefaultBlocking(
      std::shared_ptr<SEP::hypercube> hyper);

 private:
  std::vector<int> _nb;         /// Desired size for each buffer along each axis
  std::vector<int> _blocksize;  /// Chunksize to use when breaking up axes
  // compressDataType _typ;
};

}  // namespace IO
}  // namespace SEP
#endif