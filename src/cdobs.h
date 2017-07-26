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

  int DeleteObject(const int bucket_id, const int object_id,
                  std::string &err_msg);
  int DeleteObject(const int bucket_id, const std::string &name,
                  std::string &err_msg);
  int IsValidBucket(const std::string &bucket_name, std::string &err_msg);
  int EmptyBucket(const int id, std::string &err_msg);
  int NewObjectId();
  bool IsLargeObject(std::istream &src);
  int PutSmallObject(std::istream &src, const int id, const std::string &name,
                    const int bucket_id, std::string &err_msg);
  int PutLargeObject(std::istream &src, const int id, const std::string &name,
                    const int bucket_id, std::string &err_msg);

public:
  Cdobs(DbStore *store);
  int good ();

  /* Bucket operations */
  int CreateBucket(const std::string &name, std::string &err_msg);
  int DeleteBucket(const std::string &name, std::string &err_msg);
  int ListBuckets(std::vector<Bucket> &buckets, std::string &err_msg);
  int EmptyBucket(const std::string &name, std::string &err_msg);

  /* Object operations */
  int PutObject(const char *file_name, const std::string &name,
                const std::string &bucket_name, std::string &err_msg);
  int PutObject(std::istream &src, const std::string &name,
                const std::string &bucket_name, std::string &err_msg);
  int DeleteObject(const std::string &name, const std::string &bucket_name,
                  std::string &err_msg);
  int ListObjects(const std::string &bucket_name, std::vector<Object> &objects,
                  std::string &err_msg);
};

#endif
