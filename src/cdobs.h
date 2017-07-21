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
	DbStore *store;
	int bucket_count;
	int state;
public:
	Cdobs(DbStore *store);
	int good ();
	/* Bucket operations */
	int create_bucket(std::string name, std::string &err_msg);
	int delete_bucket(std::string name);
	int list_buckets(std::vector<Bucket> &buckets,
		std::string &err_msg);

	/* Object operations */
	int put_object(std::istream &src, std::string name,
		std::string bucket_name);
	int delete_object();
	int list_objects();
};

#endif
