#include <gcpBuffer.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include "google/cloud/status_or.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/oauth2/google_credentials.h"
//#include "statusor.h"
using namespace SEP::IO;

long long gcpBuffer::writeBuffer(bool keepState) {
  long long oldSize = _buf->getSize();

  std::shared_ptr<storeBase> buf;
  bufferState restore;
  if (_bufferState == UNDEFINED) throw SEPException("Bufferstate is undefined");
  if (_bufferState == ON_DISK) return 0;
  if (keepState) {
    restore = _bufferState;
    buf = _buf->clone();
  }

  changeState(CPU_COMPRESSED);
  namespace gcs = google::cloud::storage;
  gcs::ObjectWriteStream stream =
      _client.value().WriteObject(_bucketName, _name);
  std::shared_ptr<storeByte> buf2 = std::dynamic_pointer_cast<storeByte>(_buf);
  stream << buf2->toString();
  stream.Close();
  google::cloud::v0::StatusOr<gcs::ObjectMetadata> metadata =
      std::move(stream).metadata();
  if (!metadata) {
    std::cerr << "FAILURE " << _name << std::endl;
    std::cerr << metadata.status().message() << std::endl;
    throw SEPException(std::string("Trouble writing object"));
  }
  if (stream.received_hash() != stream.computed_hash())
    throw SEPException(std::string("Hashes do not match"));

  if (keepState) {
    _buf = buf;
    _bufferState = restore;
  } else {
    _bufferState = ON_DISK;
  }
  return _buf->getSize() - oldSize;
}

long long gcpBuffer::readBuffer() {
  long long oldSize = _buf->getSize();
  _modified = false;
  /*Only need to do something if sitting on disk*/
  namespace gcs = google::cloud::storage;
  if (_bufferState == ON_DISK) {
    try {
      [](gcs::Client client, std::string bucket_name, std::string object_name,
         std::shared_ptr<storeByte> buf) {
        google::cloud::v0::StatusOr<gcs::ObjectMetadata> object_metadata =
            client.GetObjectMetadata(bucket_name, object_name);

        if (!object_metadata) {
          throw std::runtime_error(object_metadata.status().message());
        }

        auto sz = object_metadata->size();

        gcs::ObjectReadStream stream =
            client.ReadObject(bucket_name, object_name);

        if (!stream.IsOpen())
          throw SEPException(std::string("stream is not open correctly"));

        buf->resize(sz);
        stream.read(buf->getPtr(), sz);

        if (stream.received_hash() != stream.computed_hash())
          throw SEPException(std::string("Hashes do not match"));
      }(std::move(_client.value()), _bucketName, _name,
        std::dynamic_pointer_cast<storeByte>(_buf));
    } catch (std::exception const &ex) {
      std::cerr << "Trouble reading from bucket " << _name << " " << ex.what()
                << std::endl;
      exit(1);
    }
    _bufferState = CPU_COMPRESSED;
  }
  if (_bufferState == UNDEFINED) throw SEPException("Bufferstate is undefined");
  return _buf->getSize() - oldSize;
}

gcpBuffer::gcpBuffer(
    const std::string &bucketName, const std::string name,
    google::cloud::v0::StatusOr<google::cloud::storage::Client> client,
    const std::vector<int> &n, const std::vector<int> &f,
    std::shared_ptr<compress> comp) {
  if (!client) throw SEPException(std::string("client is null"));
  setClient(client);
  setLoc(n, f);
  _bucketName = bucketName;
  setName(name);
  setCompress(comp);
  setBufferState(ON_DISK);
}
gcpBuffer::gcpBuffer(
    const std::string &bucketName,
    google::cloud::v0::StatusOr<google::cloud::storage::Client> client,
    const std::vector<int> &n, const std::vector<int> &f,
    std::shared_ptr<compress> comp, const bufferState state) {
  _bucketName = bucketName;
  if (!client) throw SEPException(std::string("client2is null"));
  setClient(client);

  setLoc(n, f);
  setCompress(comp);
  setBufferState(state);
  createStorageBuffer();
  _compress = comp;
}
