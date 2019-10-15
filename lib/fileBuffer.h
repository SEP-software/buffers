#ifndef file_buffer_h
#define file_buffer_h 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "buffer.h"
namespace SEP {
namespace IO {
/*!
  Buffer that is stored to conventional disk
*/
class fileBuffer : public buffer {
 public:
  //!  Create a file buffer
  /*!
    \param name The name of the buffer
    \param n  The size of the buffer along axis
    \param f The origin along each axis in global coordinates
    \param comp Compression object for buffer
  */
  fileBuffer(const std::string name, const std::vector<int> &n,
             const std::vector<int> &f,
             std::shared_ptr<compress> comp);  // Read from file
  //!  Create a file buffer
  /*!
    \param n  The size of the buffer along axis
    \param f The origin along each axis in global coordinates
    \param comp Compression object for buffer
    \param state State to set the buffer
  */
  fileBuffer(const std::vector<int> &n, const std::vector<int> &f,
             std::shared_ptr<compress> comp, const bufferState state);
  virtual void remove() override;
  //! Read a buffer
  virtual long long readBuffer() override;
  //! Write a buffer
  /*!
    \param keepState  Whether or not keep the buffer at the same state
   */
  virtual long long writeBuffer(bool keepState = false) override;
  virtual ~fileBuffer() { ; }
};
}  // namespace IO
}  // namespace SEP
#endif
