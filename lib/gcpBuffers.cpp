#include "gcpBuffers.h"
#include <sys/stat.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <future>
#include "SEPException.h"

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

  _compress = ct.getCompressionObj();

  _defaultStateSet = false;
  _projectID = getEnvVar("projectID", "NONE");
  _region = getEnvVar("region", "us-west1");
  _ntrys = std::stoi(getEnvVar("GCP_RETRYS", "10"));
  if (_projectID == std::string("NONE")) {
    throw SEPException("Must set environmental variable projectID:" +
                       _projectID);
    createBuffers(ON_DISK);
    setName(dir, false);
  }
}

gcpBuffers::gcpBuffers(std::shared_ptr<hypercube> hyper,
                       const dataType dataType, std::shared_ptr<blocking> block,
                       std::shared_ptr<compress> comp,
                       std::shared_ptr<memoryUsage> mem) {
  std::cerr << "in qgcp buffers 1" << std::endl;
  _typ = dataType;
  _compress = comp;
  _blocking = block;
  _memory = mem;
  _hyper = hyper;
  std::cerr << "ina qgcp buffers 1" << std::endl;

  if (_compress == nullptr) _compress = createDefaultCompress();
  if (_blocking == nullptr) _blocking = blocking::createDefaultBlocking(_hyper);
  if (_memory == nullptr) _memory = createDefaultMemory();
  std::cerr << "inb qgcp buffers 1" << std::endl;

  namespace gcs = google::cloud::storage;
  std::cerr << "inc qgcp buffers 1" << std::endl;

  google::cloud::v0::StatusOr<gcs::Client> client =
      gcs::Client::CreateDefaultClient();
  if (!client)
    throw(SEPException(std::string("Trouble creating default client")));

  if (!client)
    throw SEPException("_client is undefined");
  else

    blockParams v = _blocking->makeBlocks(_hyper->getNs());
  std::cerr << "inw gcp buffers 1" << std::endl;

  _projectID = getEnvVar("projectID", "NONE");
  _region = getEnvVar("region", "us-west1");
  _ntrys = std::stoi(getEnvVar("GCP_RETRYS", "10"));
  std::cerr << "in egcp buffers 1" << std::endl;

  if (_projectID == std::string("NONE")) {
    throw SEPException(
        std::string("Must set environmental variable projectID:") + _projectID);
    exit(1);
  }
  std::cerr << "iwn gcp buffers 1" << std::endl;

  createBuffers(UNDEFINED);
  std::cerr << "irn gcp buffers 1" << std::endl;

  _defaultStateSet = false;
}
void gcpBuffers::setName(const std::string &dir, const bool create) {
  int pos;
  if ((pos = dir.find("/")) == std::string::npos) {  // No subdirectory
    _bucket = dir;
    throw SEPException("Expecting bucket/dir when using GCP IO");

  } else {
    _baseName = dir;
    _bucket = _baseName.substr(0, _baseName.find("/"));
    _baseName.erase(0, _baseName.find("/") + 1);
  }

  std::vector<std::future<bool>> changes;
  std::cerr << "in set name" << std::endl;
  if (create) {
    std::cerr << "in create " << std::endl;
    namespace gcs = google::cloud::storage;
    bool found = false;

    // Create a client to communicate with Google Cloud Storage. This client
    // uses the default configuration for authentication and project id.
    namespace gcs = google::cloud::storage;
    google::cloud::v0::StatusOr<gcs::Client> client =
        gcs::Client::CreateDefaultClient();

    gcs::ListBucketsReader bucket_list =
        client->ListBucketsForProject(_projectID);
    for (auto &&bucket_metadata : bucket_list) {
      if (!bucket_metadata) {
        throw std::runtime_error(bucket_metadata.status().message());
      }
      if (bucket_metadata->name() == _bucket) found = true;
    }

    google::cloud::StatusOr<gcs::BucketMetadata> metadata =
        client->CreateBucketForProject(
            _bucket, _projectID,
            gcs::BucketMetadata().set_location(_region).set_storage_class(
                gcs::storage_class::Regional()));

    if (!found) {
      if (!metadata) {
        std::cerr << metadata.status().message() << std::endl;
        throw SEPException(std::string("Trouble creating bucket -->" +
                                       std::string(_bucket) + " <-Name"));
      }
    }

    for (auto &&object_metadata : client->ListObjects(
             _bucket, google::cloud::storage::v1::Prefix(_baseName))) {
      if (!object_metadata) {
        throw std::runtime_error(object_metadata.status().message());
      }
      changes.push_back(
          std::async(std::launch::async,
                     [&](const std::string bucket, const std::string name) {
                       client->DeleteObject(bucket, name);
                       return true;
                     },
                     object_metadata->bucket(), object_metadata->name()));
    }

    for (auto &n : changes) n.get();
  }

  for (auto i = 0; i < _buffers.size(); i++) {
    std::string hsh = std::to_string(
        std::hash<std::string>{}(std::string("/buf") + std::to_string(i)));
    //    _buffers[i]->setName(hsh.substr(0, 5) + std::string("buf") +
    //    _buffers[i]->setName(std::string("buf") +
    //                    std::to_string(i));
    _buffers[i]->setName(_baseName + std::string("/") + hsh.substr(0, 5) +
                         std::string("buf") + std::to_string(i));
    //_buffers[i]->setName(_baseName
    //+std::string("/")+ std::string("buf") + std::to_string(i));
    std::shared_ptr<gcpBuffer> b =
        std::dynamic_pointer_cast<gcpBuffer>(_buffers[i]);
    b->setBucketName(_bucket);
  }
}
void gcpBuffers::createBuffers(const bufferState state) {
  std::vector<int> ns = _hyper->getNs();
  blockParams b = _blocking->makeBlocks(ns);
  namespace gcs = google::cloud::storage;
  google::cloud::v0::StatusOr<gcs::Client> client =
      gcs::Client::CreateDefaultClient();
  if (!client) throw SEPException("client is dead on createBuffers");
  ;
  for (int i = 0; i < b._ns.size(); i++) {
    _buffers.push_back(std::make_shared<gcpBuffer>(_name, b._ns[i], b._fs[i],
                                                   _compress, state, _ntrys));
  }

  _n123blocking = b._nblocking;
  _axisBlocking = b._axesBlock;
}
