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
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/oauth2/google_credentials.h"
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
  StatusOr<gcs::Client> client = gcs::Client::CreateDefaultClient();
  if (!client)
    throw(SEPException(std::string("Trouble creating default client")));
  try {
    namespace gcs = google::cloud::storage;

    [](gcs::Client client, std::string bucket_name, std::string object_name,
       std::shared_ptr<storeByte> buf) {
      gcs::ObjectWriteStream stream =
          client.WriteObject(bucket_name, object_name);
      std::string str = buf->toString();
      std::ostreambuf_iterator<char> out_it(stream);
      std::copy(str.begin(), str.end(), out_it);
    }(std::move(client), _bucketName, _name,
      std::dynamic_pointer_cast<storeByte>(_buf));
  } catch (std::exception const &ex) {
    std::cerr << "Trouble writing to bucket " << _name << std::endl;
    exit(1);
  }

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
  if (_bufferState == ON_DISK) {
    StatusOr<gcs::Client> client = gcs::Client::CreateDefaultClient();
    if (!client)
      throw(SEPException(std::string("Trouble creating default client")));

    try {
      namespace gcs = google::cloud::storage;

      [](gcs::Client client, std::string bucket_name, std::string object_name,
         std::shared_ptr<storeByte> buf) {
        gcs::ObjectReadStream stream =
            client.ReadObject(bucket_name, object_name);

        std::string data(std::istreambuf_iterator<char>(stream), {});
        buf->fromString(data);
      }(std::move(client), _bucketName, _name,
        std::dynamic_pointer_cast<storeByte>(_buf));
    } catch (std::exception const &ex) {
      std::cerr << "Trouble writing to bucket " << _name << std::endl;
      exit(1);
    }
    _bufferState = CPU_COMPRESSED;
  }
  if (_bufferState == UNDEFINED)) throw SEPException("Bufferstate is undefined");
  return _buf->getSize() - oldSize;
}

gcpBuffer::gcpBuffer(const std::string &bucketName, const std::string name,
                     const std::vector<int> &n, const std::vector<int> &f,
                     std::shared_ptr<compress> comp) {
  setLoc(n, f);
  _bucketName = bucketName;
  setName(name);
  setCompress(comp);
  setBufferState(ON_DISK);
}
gcpBuffer::gcpBuffer(const std::string &bucketName, const std::vector<int> &n,
                     const std::vector<int> &f, std::shared_ptr<compress> comp,
                     const bufferState state) {
  _bucketName = bucketName;

  setLoc(n, f);
  setCompress(comp);
  setBufferState(state);
  createStorageBuffer();
  _compress = comp;
}
