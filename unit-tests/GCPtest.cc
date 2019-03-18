#include <gtest/gtest.h>  // googletest header file
#include <fstream>
#include <iostream>
#include <string>
#include "gcpBuffers.h"
#include "ioTypes.h"
#include "google/cloud/status_or.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/oauth2/google_credentials.h"
using std::string;
using namespace SEP::IO;

std::shared_ptr<storeFloat> array() {
  std::vector<float> ar(8000);
  size_t ii = 0;

  for (auto i3 = 0; i3 < 20; i3++) {
    for (auto i2 = 0; i2 < 20; i2++) {
      for (auto i1 = 0; i1 < 20; i1++, ii++) {
        ar[ii] = (float)rand() / ((float)RAND_MAX) + i1 * .4 + 5.;
      }
    }
  }
  std::shared_ptr<storeFloat> store(new storeFloat(8000, ar.data()));
  return store;
}


TEST(TESTBucketCreation, gcpBuffers) {


  std::vector<SEP::axis> axes;
  long long n=200;
  long long n123=1;
  int ndim=4;
  std::vector<int> ns(ndim,n),fs(ndim,0),js(ndim,1);
  for (int i=0;i < ndim; i++){
	  n123=n123*n;
	  axes.push_back(SEP::axis(n)); 
  }

  std::shared_ptr<SEP::hypercube> hyper(new SEP::hypercube(axes));

  std::string bucket=std::string("testbucket993");


   
  std::shared_ptr<SEP::IO::blocking>  block=SEP::IO::blocking::createDefaultBlocking(hyper);
  SEP::IO::gcpBuffers gcp(hyper,SEP::DATA_FLOAT,block);
  ASSERT_NO_THROW(gcp.setName(bucket,true));

 
  std::vector<float> vals(n123);

  ASSERT_NO_THROW(gcp.putWindow(ns,fs,js,vals.data()));
 ASSERT_NO_THROW(gcp.changeState(ON_DISK));




    
// namespace gcs = google::cloud::storage;
 //   google::cloud::v0::StatusOr<gcs::Client> client=gcs::Client::CreateDefaultClient();
//	        client.value().DeleteBucket(bucket);
}
