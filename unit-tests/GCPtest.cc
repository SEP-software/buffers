#include <gtest/gtest.h>  // googletest header file
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include "gcpBuffers.h"
#include "google/cloud/status_or.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/oauth2/google_credentials.h"
#include "ioTypes.h"
using namespace std::chrono;

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
  long long n = 200;
  long long n123 = 1;
  int ndim = 4;
  std::vector<int> ns(ndim, n), fs(ndim, 0), js(ndim, 1);
  for (int i = 0; i < ndim; i++) {
    n123 = n123 * n;
    axes.push_back(SEP::axis(n));
  }

  std::shared_ptr<SEP::hypercube> hyper(new SEP::hypercube(axes));

  std::string bucket = std::string("testbucket996");
  std::string bucket1 = bucket + std::string("/dataset1");
  std::string bucket2 = bucket + std::string("/dataset2");


  // Create Default blocking
//  std::shared_ptr<SEP::IO::blocking> block =
//      SEP::IO::blocking::createDefaultBlocking(hyper);

   std::vector<int> big(4,40),bs(4,2) ;
	   big[0]=100;
  std::shared_ptr<SEP::IO::blocking> block( new SEP::IO::blocking(bs,big));
	   
   
  float *vals=new float[n123];

  Json::Value val;
    high_resolution_clock::time_point t2,t3,t1;
  for(int i=0;i < 8; i++){

  // Create simple file and write to disk
  SEP::IO::gcpBuffers gcp(hyper, SEP::DATA_FLOAT, block);
  ASSERT_NO_THROW(gcp.setName(bucket1+std::to_string(i), true));


//  std::vector<float> vals(n123);

    t1 = high_resolution_clock::now();

  ASSERT_NO_THROW(gcp.putWindow(ns, fs, js, vals));
   t2 = high_resolution_clock::now();


  ASSERT_NO_THROW(gcp.changeState(ON_DISK));
    high_resolution_clock::time_point t3 = high_resolution_clock::now();

      auto d1 = duration_cast<microseconds>(t2 - t1).count();
        auto d2 = duration_cast<microseconds>(t3 - t2).count();
	  double s1=(double) n123*4/d1;
	  double s2=(double) n123*4/d2;

	  std::cerr <<"To buffer " << s1 << " MB/s " << std::endl; ;
	      std::cerr << "To cloud " <<s2 << " MB/s " << std::endl;
  val["blocking"] = block->getJsonDescription();
  val["compression"] = gcp.getCompressObj()->getJsonDescription();

  }

  // Create a second directory in same bucket
  /*
  SEP::IO::gcpBuffers gcp2(hyper, SEP::DATA_FLOAT, block);
  ASSERT_NO_THROW(gcp2.setName(bucket2, true));

  ASSERT_NO_THROW(gcp2.putWindow(ns, fs, js, vals.data()));
  ASSERT_NO_THROW(gcp2.changeState(ON_DISK));

  */
  // Now read the bucket from disk
  
	      float tot=0;
	    for(int i=0; i < 7; i++){
		   float *vals2=new float [n123];
  SEP::IO::gcpBuffers gcp3(hyper, bucket1+std::to_string(i), val);
    t2 = high_resolution_clock::now();


//  ASSERT_NO_THROW(gcp3.getWindow(ns, fs, js, vals2.data()));
//  ASSERT_NO_THROW(gcp3.changeState(CPU_COMPRESSED));
  ASSERT_NO_THROW(gcp3.getWindow(ns, fs, js, vals2));


    t3 = high_resolution_clock::now();
     auto  d2 = duration_cast<microseconds>(t3 - t2).count();
      std::cerr << "From cloud " << (double) n123 * 4 /d2 << " MB/s "
	                  << std::endl;
        ;
	tot+=d2;
	    }

	    std::cerr<<n123*28/tot<<" average speed"<<std::endl;


  namespace gcs = google::cloud::storage;
  google::cloud::v0::StatusOr<gcs::Client> client =
      gcs::Client::CreateDefaultClient();
  client.value().DeleteBucket(bucket);
}
