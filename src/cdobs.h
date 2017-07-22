#ifndef CDOBS_H
#define CDOBS_H

#include <string>
#include <vector>
#include <iostream>
#include "dbstore.h"

 /* The cdobs class : The Ceph Database object storage. 
  * Used in place of librados to store RGW(S3) objects in an 
  * sql database */ 

class Cdobs {
private:
	DbStore *store_;
	int bucket_count_;
	int state_;
public:
	Cdobs(DbStore *store);
	int good ();
	/* Bucket operations */
	int CreateBucket(std::string name, std::string &err_msg);
	int DeleteBucket(std::string name);
	int ListBuckets(std::vector<Bucket> &buckets,
		std::string &err_msg);

	/* Object operations */
	int PutObject(std::istream &src, std::string name,
		std::string bucket_name, std::string &err_msg);
	int DeleteObject();
	int ListObjects();
};

#endif
