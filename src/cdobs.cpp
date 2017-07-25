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

Cdobs::Cdobs(DbStore *store_): state_(S_NOINIT) {
  this->store_ = store_;
  state_ = store_->good() ? S_GOOD : !S_GOOD;
  bucket_count_ = store_->GetBucketCount();
}

int Cdobs::good () {
  return (state_ == S_GOOD);
}

int Cdobs::CreateBucket(string name, string &err_msg) {
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

int Cdobs::EmptyBucket(const int id, string &err_msg) {
  vector<int> object_ids;
  store_->SelectObjectIdsInBucket(id, object_ids);
  for (auto it = object_ids.begin(); it != object_ids.end(); ++it) {
    if (DeleteObject(id, *it, err_msg)) {
      return -1;
    }
  }
  return 0;
}

// Also needs to delete all objects first.
int Cdobs::DeleteBucket(const string &name, string &err_msg) {
  int id;
  if ((id = IsValidBucket(name, err_msg)) < 0) {
    return -1;
  }
  int ret_value = 0;
  store_->BeginTransaction();
  int rc_empty = EmptyBucket(id, err_msg);
  if (!rc_empty) {
    int err = store_->DeleteBucketEntry(id);
    if (err) {
      ret_value = -1;
    }
  }
  else {
    err_msg = kErrEmptyingBucket + name;
    ret_value = -1;
  }
  if (ret_value) {
    store_->RollbackTransaction();
  }
  else {
    store_->CommitTransaction();
  }
  return ret_value;
}

int Cdobs::ListBuckets(vector<Bucket> &buckets,
                      string &err_msg) {
  int count = store_->SelectAllBuckets(buckets, err_msg);
  return count; 
}

int Cdobs::IsValidBucket(const string &bucket_name, string &err_msg) {
  int bucket_id = store_->GetBucketId(bucket_name.c_str());
  if (bucket_id < 0) {
    err_msg = kErrInvalidBucketName + " " + bucket_name;
  }
  return bucket_id;
}

bool Cdobs::IsLargeObject(istream &src) {
  char c; bool ret_value = false;
  src.seekg(kMaxObjectSize, ios::beg);
  src >> c;
  if (!src.eof()) {
    ret_value = true;
  }
  // Reset the stream
  src.clear();
  src.seekg(0, ios::beg);
  return ret_value;
}

int Cdobs::PutObject(const char *file_name, const string &name,
                    const string &bucket_name, string &err_msg) {
  ifstream src;
  if (file_name) {
    src.open(file_name, ios::binary);
  }

  if (!file_name || !src.good()) {
    err_msg = kErrInvalidFile + string(file_name);
    return 1;
  }

  return PutObject(src, name, bucket_name, err_msg);
}

int Cdobs::NewObjectId() {
  vector<int> object_ids;
  dout << object_ids.size() << endl;
  if (store_->SelectAllObjectIds(object_ids)) {
    return -1;
  }
  int id;
  if (object_ids.size()) {
    id = object_ids[object_ids.size() - 1] + 1;
  }
  else {
    id = 1;
  }
  return id;
}

int Cdobs::PutObject(istream &src, const string &name,
                    const string &bucket_name, string &err_msg) { 
  int bucket_id;
  if ((bucket_id = IsValidBucket(bucket_name, err_msg)) < 0) {
    dout << "Invalid Bucket" << endl;
    return -1;
  }
  int id = store_->GetObjectId(bucket_id, name.c_str()), size;
  if (id > 0) {
    dout << "Obj exist" << endl;
    err_msg = kErrObjectAlreadyExists + name;
    return -1;
  }
  else {
    id = NewObjectId();
    dout << "id of new object: " << id << endl;
    if (id < 0) {
      return -1;
    }
    // Actually put the object
    size = IsLargeObject(src) ?
          PutLargeObject(src, id, name, bucket_id, err_msg)
         :PutSmallObject(src, id, name, bucket_id, err_msg);
  }

  return size;
}

int Cdobs::PutSmallObject(istream &src, const int id, const string &name,
                          const int bucket_id, string &err_msg) {
  store_->BeginTransaction();
  int size = 0;
  char ctime[kMaxTimeLength];
  int writ = GetCurrentTime(ctime, kMaxTimeLength);
  int rc_cr = store_->CreateObjectEntry(id, name.c_str(), bucket_id,
                                        ctime, 0, err_msg);
  if (rc_cr) {
    dout << "create object entry failed" << endl;
    size = -1; 
  }
  else {
    size = store_->PutObjectData(id, src, err_msg);
    if (size > 0) {
      store_->UpdateObjectSize(id, size);
      if (store_->CommitTransaction()) {
        size = -1;
      }    
    }
  }

  if (size < 0) {
    store_->RollbackTransaction();
  }
  dout << "Size is: " << size << endl;
  return size;
}

int Cdobs::PutLargeObject(istream &src, const int id, const string &name,
                          const int bucket_id, string &err_msg) {
  store_->BeginTransaction();
  int total_size = 0;
  char ctime[kMaxTimeLength];
  int writ = GetCurrentTime(ctime, kMaxTimeLength);
  int rc_cr = store_->CreateObjectEntry(id, name.c_str(), bucket_id,
                                        ctime, 1, err_msg);
  if (rc_cr) {
    total_size = -1;
  }
  else {
    int segment = 1, seg_size = 0;
    while (!src.eof()) {
      seg_size =
        store_->PutObjectData(id, src, kSegmentSize, segment++, err_msg);     
      if (seg_size >= 0) {
        total_size += seg_size;
      }
      else {
        total_size = -1;
        break;
      }
    }
    if (total_size > 0) {
      store_->UpdateObjectSize(id, total_size);
      if (store_->CommitTransaction()) {
        total_size = -1;
      }    
    } 
  }    

  if (total_size < 0) {
    store_->RollbackTransaction();
  }
  return total_size;
}

int Cdobs::DeleteObject(const int bucket_id, const int object_id,
                        string &err_msg) {
  int rc_dd = store_->DeleteObjectData(object_id);
  int ret_value = 0;
  if (!rc_dd) {
    int rc_de = store_->DeleteObjectEntry(object_id);
    if (rc_de) {
      err_msg = "Error deleting object entry";
      ret_value = 1;
    }
  }
  else {
    err_msg = "Error deleting object data";
    ret_value = 1;
  }

  return ret_value;
}

int Cdobs::DeleteObject(const int bucket_id, const string &name,
                        string &err_msg) {
  int object_id = store_->GetObjectId(bucket_id, name.c_str());
  if (object_id < 0) {
    err_msg = kErrInvalidObjectName + name;
    return -1;
  }

  return DeleteObject(bucket_id, object_id, err_msg);
}

int Cdobs::DeleteObject(const string &bucket_name, const string &name,
                        string &err_msg) {
  store_->BeginTransaction();
  int bucket_id = store_->GetBucketId(bucket_name.c_str());
  if (bucket_id < 0) {
    err_msg = kErrInvalidBucketName + bucket_name;
    return -1;
  }

  int rc_del = DeleteObject(bucket_id, name, err_msg);
  if (rc_del) {
    store_->RollbackTransaction();
    return -1;
  }
  else {
    store_->CommitTransaction();
    return 0;
  }
}

int Cdobs::ListObjects(
    const string &bucket_name, vector<Object> &objects,
    string &err_msg) {
  int bucket_id = store_->GetBucketId(bucket_name.c_str());
  if (bucket_id < 0) {
    err_msg = kErrInvalidBucketName + bucket_name;
    return -1;
  }
  
  return store_->SelectObjectsInBucket(bucket_id, objects, err_msg);
}
