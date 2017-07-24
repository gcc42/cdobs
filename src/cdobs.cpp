#include "cdobs.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include "config.h"
#include "utils.h"
#include "dbstore.h"
#include "dberror.h"

using namespace std;

Cdobs::Cdobs (DbStore *store_): state_(S_NOINIT) {
  this->store_ = store_;
  state_ = store_->good() ? S_GOOD : !S_GOOD;
  bucket_count_ = store_->GetBucketCount();
}

int Cdobs::good () {
  return (state_ == S_GOOD);
}

int Cdobs::CreateBucket (string name, string &err_msg) {
  char ctime[kMaxTimeLength];
  // Get time as an "YYYY-MM-DD HH:MM:SS" format string
  int writ = GetCurrentTime(ctime, kMaxTimeLength);
  int bucket_id = ++bucket_count_;
  int intial_obj_count = 0;

  const char *bname = name.c_str();
  // Check if bucket name already exists
  if (store_->GetBucketId(bname) != -1) {
    err_msg = kErrBucketAlreadyExists;
    return 1;
  }

  int err = store_->InsertBucket(bucket_id, bname, ctime,
    intial_obj_count);
  return err;
}

// Also needs to delete all objects first.
int Cdobs::DeleteBucket (string name) {
  int id = store_->GetBucketId(name.c_str());
  if (id == -1) {
    return 1;
  }
  store_->EmptyBucket(id);
  int err = store_->DeleteBucket(id);
  return err;
}

int Cdobs::ListBuckets (vector<Bucket> &buckets,
  string &err_msg) {
  int count = store_->SelectAllBuckets(buckets, err_msg);
  return count; 
}

int Cdobs::PutObject (istream &src, string name,
  string bucket_name, string &err_msg) {
  
  char ctime[kMaxTimeLength];
  // Get time as an "YYYY-MM-DD HH:MM:SS" format string
  int writ = GetCurrentTime(ctime, kMaxTimeLength);
  int bucket_id = store_->GetBucketId(bucket_name.c_str());
  if (bucket_id < 0) {
    err_msg = kErrInvalidBucketName + " " + bucket_name;
    return -1; // Also set bucket not exists error code.
  }
  int id = store_->CreateObject(name.c_str(), 
    bucket_id, ctime, err_msg);
  if (id < 0) {
    return -1;
  }
  int size = store_->PutObject(src, id, err_msg);
  if (size == -1) {
    return -1; // Error status to object to large
  }
  store_->UpdateObjectSize(id, size);
  return size;
}

int Cdobs::DeleteObject () {
  // To be implemented.
}

int Cdobs::ListObjects () {
  // To be implemented.
}
