#ifndef BUFFERS_H
#define BUFFERS_H 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "blocking.h"
#include "buffer.h"
#include "compress.h"
#include "hypercube.h"
#include "memoryUsage.h"
namespace SEP {
namespace IO {

/*!
  Break a hypercube into many sub-hypercube
*/
class buffers {
 public:
  buffers() { ; }
  //! Get window of hypercube and store it in buf
  /*!
    \param nw,fw,jw Sampling of window to grab
    \param buf Where to store data
    */
  void getWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw, void *buf);
  //! Put window of hypercube stored in buf to different buffers
  /*!
    \param nw,fw,jw Sampling of window to grab
    \param buf Where to store data
    */

  void putWindow(const std::vector<int> &nw, const std ::vector<int> &fw,
                 const std::vector<int> &jw, const void *buf);

  //! Create buffers
  /*!
    \param state for the created buffers
    */

  virtual void createBuffers(const bufferState state) = 0;
  //! Change the state of all the buffers
  /*!
    \param state to change
    */

  void changeState(const bufferState state);
  //! Get a description of the buffers object in JSON format
  Json::Value getDescription();
  //! Update the amount of memory used by the buffers
  /*!
    \param change How the amount of memory used by the buffers has chasnged
    */

  void updateMemory(const long change);
  //! Set the rules for how handle memory from the buffer object
  /*!
    \param mem Memory usage object
    */

  void setMemoryUsage(std::shared_ptr<memoryUsage> mem) { _memory = mem; }
  //! Create the default compress object
  std::shared_ptr<compress> createDefaultCompress();
  //! Create the default memory object
  std::shared_ptr<memoryUsage> createDefaultMemory();
  //! Set the default state for buffers. The state buffers will be always be
  //! converted to
  /*!
    \param state  The state to set all the buffers
    */

  void setDefaultState(const SEP::IO::bufferState state) {
    _defState = state;
    _defaultStateSet = true;
  }
  //! Set the name for the dataset
  /*!
    \param dir The directory for the dataset
    \param create Whether or not create the directory
    */

  virtual void setName(const std::string &dir, const bool create) = 0;
  //! Given a window call return buffers that are modified
  /*!
    \param nw,fw,jw Window parameters
    */
  std::vector<int> parsedWindows(const std::vector<int> &nw,
                                 const std ::vector<int> &fw,
                                 const std::vector<int> &jw);
  //! Return a specific buffer (useful for debugging)
  /*!
    \param ibuf - Buffer number
    */

  std::shared_ptr<storeBase> getSpecificStore(int ibuf) {
    return _buffers[ibuf]->getStorePtr();
  }
  //! Return the current compresison object
  std::shared_ptr<compress> getCompressObj() { return _compress; }

 protected:
  bool _defaultStateSet;  ///< Wthether of not the default state has been set
  SEP::IO::bufferState _defState;        ///< Default state for buffers
  std::shared_ptr<blocking> _blocking;   ///< Blocking oject
  std::shared_ptr<memoryUsage> _memory;  ///< Memory object
  std::shared_ptr<compress> _compress;   ///< Compression object
  dataType _typ;                         ///< Data type for buffer
  std::shared_ptr<hypercube> _hyper;     ///< Hypercube describing dataset
  std::vector<std::shared_ptr<buffer>> _buffers;  ///< Collection of buffers
  std::vector<int> _lastUsed;                     ///< Last used buffers
  std::vector<std::vector<int>> _axisBlocking;    ///< How to axes are blocked
  std::vector<int> _n123blocking;                 ///< Number of blocks
  std::string _name;                              ///< Directory
};
}  // namespace IO
}  // namespace SEP
#endif
