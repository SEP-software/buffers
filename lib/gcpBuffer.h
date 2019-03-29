#ifndef gcp_buffer_h
#define gcp_buffer_h 1
#include <store.h>
#include <cstring>
#include <memory>
#include <sstream>
#include <vector>
#include "buffer.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/status_or.h"
namespace SEP {
namespace IO {

class gcpBuffer : public buffer {
 public:
  gcpBuffer(const std::string &bucketName, const std::string name,
		  google::cloud::v0::StatusOr<google::cloud::storage::Client>
		  client,
            const std::vector<int> &n, const std::vector<int> &f,
            std::shared_ptr<compress> comp);  // Read from file
  gcpBuffer(const std::string &bucketName, 
		  google::cloud::v0::StatusOr<google::cloud::storage::Client> client,
		  const std::vector<int> &n,
            const std::vector<int> &f, std::shared_ptr<compress> comp,
            const bufferState state);

  
  void setClient(google::cloud::v0::StatusOr<google::cloud::storage::Client> client) {_client=client;}
  void setBucketName(const std::string nm){ _bucketName=nm;}
  virtual long long readBuffer() override;
  virtual long long writeBuffer(bool keepState = false) override;
  virtual ~gcpBuffer() { ; }

   

 private:
  std::string _bucketName;
  google::cloud::v0::StatusOr<google::cloud::storage::Client> _client;
};
}  // namespace IO
}  // namespace SEP
#endif
