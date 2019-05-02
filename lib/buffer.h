#ifndef buffer_h
#define buffer_h 1
#include <store.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "SEPException.h"
#include "compress.h"
namespace SEP {
namespace IO {
//! Different status for buffers

enum bufferState {
  UNDEFINED,         /// Undefined status
  CPU_COMPRESSED,    /// Buffer is compressed
  CPU_DECOMPRESSED,  /// Buffer is decompressed
  ON_DISK            /// Buffer is on disk
};

//! Convert buffer stat to a readable string
std::string bufferStateToString(const bufferState &state);

/*!
  Virtual buffer object
*/

class buffer {
 public:
  buffer() { ; }
  //!= Create a storage buf, sized to 0
  void createStorageBuffer() {
    std::shared_ptr<storeByte> bb(new storeByte(0));
    _buf = bb;
  }
  //! Set the state of the buffer
  /*!
    \param state State to set buffer (disk, compressed, etc)
  */
  void setBufferState(const bufferState state) { _bufferState = state; }
  //! Set the compression approach to used for the buffer
  /*!
    \param comp Compression object
  */
  void setCompress(std::shared_ptr<compress> comp) { _compress = comp; }
  //! Set the  location of the buffer inside the global hypercube
  /*!
    \param n Vector of size of buffer along each axis
    \param f Vector of origin of buffer along each axis
  */
  void setLoc(const std::vector<int> &n, const std::vector<int> &f) {
    _f = f;
    _n = n;
    _n123 = 1;
    for (int i : _n) _n123 *= i;
    setBlock();
  }
  //! Set the name of the file/object for buffer
  /*!
    \param name of file/object
  */
  void setName(const std::string name) {
    _name = name;
    _nameSet = true;
  }
  //! Return the file name
  std::string getFileName() {
    if (!_nameSet) throw SEPException(std::string("Name of file not set"));
    size_t found = _name.find_last_of("/\\");

    return _name.substr(found + 1);
  }
  //!  Set block (1,n[0],n[0]*n[1])
  void setBlock() {
    _block.push_back(1);
    for (size_t i = 0; i < _n.size(); i++) _block.push_back(_block[i] * _n[i]);
  }
  //! Virtual function to read buffer from disk
  virtual long long readBuffer() = 0;
  //! Write buffer to disk
  /*!
    \param keepState  Keep the current state of the buffer after write
  */
  virtual long long writeBuffer(bool keepState = false) = 0;
  //! Return the buffer
  /*!
    \param  buf  Buffer to return
    \param finalState  Final state for buffer
  */
  virtual long long getBufferCPU(std::shared_ptr<storeBase> buf,
                                 const bufferState finalState);
  //! Store the buffer
  /*!
    \param  buf  Buffer to return
    \param finalState  Final state for buffer
  */
  virtual long long putBufferCPU(std::shared_ptr<storeBase> buf,
                                 const bufferState finalState);
  //! Store data into buffer storage
  /*!
   \param nwL Local (buffer) number of elements along each axis
   \param fwL Local Origin of element along each axis
   \param jwL Local skip of element along each axis
    \param nwG Global size of buffer
    \param fwG Global origin element along each axis
    \param blockG Block of global buffer
    \param buf  Buffer to store
    \param finalState  Final state for buffer
  */
  long long getWindowCPU(const std::vector<int> &nwL,
                         const std ::vector<int> &fwL,
                         const std::vector<int> &jwL,
                         const std::vector<int> &nwG,
                         const std ::vector<int> &fwG,
                         const std::vector<int> &blockG, void *buf,
                         const bufferState finalState);
  //! Grab data into buffer storage
  /*!
   \param nwL Local (buffer) number of elements along each axis
   \param fwL Local Origin of element along each axis
   \param jwL Local skip of element along each axis
    \param nwG Global size of buffer
    \param fwG Global origin element along each axis
    \param blockG Block of global buffer
    \param buf  Buffer to fill
    \param finalState  Final state for buffer
  */
  long long putWindowCPU(
      const std::vector<int> &nwL, const std ::vector<int> &fwL,
      const std::vector<int> &jwL, const std::vector<int> &nwG,
      const std ::vector<int> &fwG, const std::vector<int> &blockG,
      const void *buf,
      const bufferState finalState);  //! Store  Given window parameters convert
                                      //! them for local buffer
  /*!
   \param nw Number of samples each axis needed for global operation
   \param fw Origin along  each axis needed for global operation
   \param jw Skip of element along each axis for global operation
    \param n_w Local number of samples for each axis
    \param f_w Local origin for each axis
    \param j_w Skip parameter for local buffer
    \param nwG Given complete buffer, number of samples each axis
    \param fwG  Given complete buffer, origin of samples each axis
    \param blockG 1,nwG[0],nw[0]*nwG[1],
  */
  size_t localWindow(const std::vector<int> &nw, const std::vector<int> &fw,
                     const std::vector<int> &jw, std::vector<int> &n_w,
                     std::vector<int> &f_w, std::vector<int> &j_w,
                     std::vector<int> &nwG, std::vector<int> &fwG,
                     std::vector<int> &blockG) const;
  //! Change the state of the buffer
  /*!
   \param state to change gthe buffer to
   */
  long changeState(const bufferState state);
  //! Get the blocks for the buffer

  std::vector<int> getBlock() { return _block; }
  //! Return the storage for the buffer
  std::shared_ptr<storeBase> getStorePtr() { return _buf; }
  virtual ~buffer() { ; }

  std::vector<int>
      _f;  ///< Origin of buffer in global coordinates along each axis
  std::vector<int> _n;      ///< Size of buffer along each axis
  std::vector<int> _block;  ///< Block for buffer (1,n[0],n[0]*1)

 protected:
  bufferState _bufferState = UNDEFINED;  ///< Current buffer state
  std::shared_ptr<storeBase> _buf=NULL;      ///< Storage for buffer
  std::shared_ptr<compress> _compress;  ///< Compression object
  std::string _name;                    ///< Name for buffer
  bool _nameSet;           ///< Whether or not buffer's name has been set
  bool _modified = false;  ///< Wheter buffer has been modified
  int _ibuf;               ///< Buffer number
  long long _n123;         ///< Number of element in buffer
};
}  // namespace IO
}  // namespace SEP
#endif
