#include "gcpBuffers.h"
#include <sys/stat.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

#include <tbb/tbb.h>
#include "compressTypes.h"
#include "google/cloud/status_or.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/oauth2/google_credentials.h"
#include "memoryAll.h"
#include "nocompress.h"
#include "simpleMemoryLimit.h"
using namespace SEP::IO;

gcpBuffers::gcpBuffers(const std::shared_ptr<hypercube> hyper,
                       const std::string dir, const Json::Value &des,
                       std::shared_ptr<memoryUsage> mem) {
  _hyper = hyper->clone();
  if (des["blocking"].isNull()) {
    throw SEPException(
        std::string("Trouble grabbing blocking from parameters"));
  }

  _blocking.reset(new blocking(des["blocking"]));

  if (des["compression"].isNull()) {
    throw SEPException(
        std::string("Trouble grabbing compression from parameters"));
  }

  _memory = mem;
  if (!_memory) {
    _memory = createDefaultMemory();
  }

  SEP::IO::compressTypes ct = compressTypes(des["compression"]);

    namespace gcs = google::cloud::storage;
    google::cloud::v0::StatusOr<gcs::Client> client =
        gcs::Client::CreateDefaultClient();
    if (!client)
      throw(SEPException(std::string("Trouble creating default client")));

    _client=client;
  _compress = ct.getCompressionObj();

  _defaultStateSet = false;

  createBuffers(ON_DISK);
  setName(dir, false);

  _projectID = getEnvVar("projectID", "NONE");
  _region = getEnvVar("region", "us-west1");
  if (_projectID == std::string("NONE")) {
    std::cerr << "Must set environmental variable projectID" << _projectID << std::endl;
    exit(1);
  }
}

gcpBuffers::gcpBuffers(std::shared_ptr<hypercube> hyper,
                       const dataType dataType, std::shared_ptr<blocking> block,
                       std::shared_ptr<compress> comp,
                       std::shared_ptr<memoryUsage> mem) {
  _typ = dataType;
  _compress = comp;
  _blocking = block;
  _memory = mem;
  _hyper = hyper;
  if (_compress == nullptr) _compress = createDefaultCompress();
  if (_blocking == nullptr) _blocking = blocking::createDefaultBlocking(_hyper);
  if (_memory == nullptr) _memory = createDefaultMemory();

  blockParams v = _blocking->makeBlocks(_hyper->getNs());
  createBuffers(UNDEFINED);
  _defaultStateSet = false;

  _projectID = getEnvVar("projectID", "NONE");
  _region = getEnvVar("region", "us-west1");
  if (_projectID == std::string("NONE")) {
    std::cerr << "Must set environmental variable " << _projectID << std::endl;
    exit(1);
  }
    namespace gcs = google::cloud::storage;
    google::cloud::v0::StatusOr<gcs::Client> client =
        gcs::Client::CreateDefaultClient();
    if (!client)
      throw(SEPException(std::string("Trouble creating default client")));

    _client=client;
}

void gcpBuffers::setName(const std::string &dir, const bool create) {


	int pos;
  if ((pos = dir.find("/")) == std::string::npos) {  // No subdirectory
    _bucket = dir;
    _baseName = "";
  } else {
    _baseName=dir;
    _bucket = _baseName.substr(0, _baseName.find("/"));
    _baseName.erase(0, _baseName.find("/") + 1);
  }

  if (create) {
    namespace gcs = google::cloud::storage;
    bool found=false;

    // Create a client to communicate with Google Cloud Storage. This client
    // uses the default configuration for authentication and project id.


    gcs::ListBucketsReader bucket_list = _client->ListBucketsForProject(_projectID);
      for (auto&& bucket_metadata : bucket_list) {
          if (!bucket_metadata) {
	        throw std::runtime_error(bucket_metadata.status().message());
	    }
          if(bucket_metadata->name()==_bucket) found=true;
      }



    google::cloud::StatusOr<gcs::BucketMetadata> metadata =
        _client->CreateBucketForProject(
            _bucket, _projectID,
            gcs::BucketMetadata().set_location(_region).set_storage_class(
                gcs::storage_class::Regional()));

     if(!found){
    if (!metadata) {
        std::cerr << metadata.status().message() << std::endl;
        throw SEPException(std::string("Trouble creating bucket "));
      }
     }
  }
  for (auto i = 0; i < _buffers.size(); i++) { 
    _buffers[i]->setName(_baseName+ std::string("/buf") + std::to_string(i));
    std::shared_ptr<gcpBuffer> b =
        std::dynamic_pointer_cast<gcpBuffer>(_buffers[i]);
    b->setBucketName(_bucket);
  }
}
void gcpBuffers::createBuffers(const bufferState state) {
  std::vector<int> ns = _hyper->getNs();
  blockParams b = _blocking->makeBlocks(ns);
  for (int i = 0; i < b._ns.size(); i++) {
    _buffers.push_back(std::make_shared<gcpBuffer>(_name,_client, b._ns[i], b._fs[i],
                                                   _compress, state));

  }

  std::cerr<<_buffers[0]->_n[0]<<" "<<_buffers[0]->_n[1]<<" "<<_buffers[0]->_n[2]<<" "<<_buffers[0]->_n[3]<<std::endl;

  _n123blocking = b._nblocking;
  _axisBlocking = b._axesBlock;
}
