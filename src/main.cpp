#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "config.h"
#include "init.h"
#include "create.h"
#include "dbstore.h"
#include "cdobs.h"
#include "dberror.h"

using namespace std;

static const string kHelpBucket = 
"Usage: cdobs bucket \
[help | create name | \
list name | delete name]\n\
where name is the bucket_name";

static const string kHelpCdobs = 
"Usage: cdobs \
[help | bucket | object]\n\
Hit one of the commands to know more \
about them";

static const string kHelpObject = 
"Usage: cdobs object \
[put -b bucket_name -f file_name object_name]";

void Help () {
  cout << kHelpCdobs << endl;
}

void BucketHelp () {
  cout << kHelpBucket << endl;
}

void ObjectHelp () {
  cout << kHelpObject << endl;
}

int comInit () {
  string err_msg;
  int rc = SetupDatabase(err_msg);
  if (rc) {
    cerr << "ERROR: " << kErrInitFailed
    << " " << err_msg << endl;
    return 1;
  }
  return 0;
}


int comCreateBucket (Cdobs *const cdobs, int argc,
  char **argv) {
  int ret_value = 0;
  string bucket_name(argv[2]), err_msg;
  int rc_cb = cdobs->CreateBucket(bucket_name, err_msg);
  if (rc_cb) {
    ret_value = 1;    
    cerr << "ERROR: " << kErrCreateBucketFailed
    << err_msg << endl;
  }   

  return ret_value;
}

int comListBuckets (Cdobs *const cdobs) {
  vector<Bucket> buckets;
  string err_msg;
  int rc = cdobs->ListBuckets(buckets, err_msg);
  if (rc) {
    cout << "ERROR: " << err_msg << endl;
    return 1;
  }
  if (buckets.empty()) {
    cout << "No buckets in storage" << endl;
  }
  else {
    for (auto b = buckets.begin(); b != buckets.end(); ++b) {
      cout  << b->id << "   " << b->name << endl;
    }   
  }
  return 0;
}

int comBucket (Cdobs *const cdobs, int argc, char **argv) {
  if (argc < 2) {
    return comListBuckets(cdobs);
  }

  string arg1(argv[1]); int rc_op;
  if (arg1 == "create") {
    rc_op = comCreateBucket(cdobs, argc, argv);
  }
  else {
    BucketHelp();
    return 1;
  }
}

int comPutObject (Cdobs *const cdobs, int argc,
  char **argv) {
  string bucket_name, file_name, object_name;
  for (int i = 2; i < argc; ++i) {
    if (!std::strcmp(argv[i], "-b")) {
      bucket_name = argv[i + 1];
      i++;
    }
    else if (!std::strcmp(argv[i], "-f")) {
      file_name = argv[i + 1];
      i++;
    }
    else if (i == argc - 1) {
      object_name = argv[i];
    }
  }

  if (bucket_name == "" || object_name == ""
    || file_name == "") {
    cout << kErrInvalidSyntax << endl;
    ObjectHelp();
    return 1;
  }

  dout << bucket_name << " " << object_name << "  "
  << file_name << endl;

  ifstream src(file_name.c_str(), ios::binary);
  if (!src.good()) {
    cout << "ERROR: " << kErrPutObjectFailed
    << kErrInvalidFile << file_name << endl;
    return 1;
  }

  string err_msg;
  int rc = cdobs->PutObject(src, object_name,
    bucket_name, err_msg);
  if (rc) {
    cout << "ERROR: " << kErrPutObjectFailed
    << err_msg << endl;
    return 1;
  }
  return 0;
}

int comObject (Cdobs *const cdobs, int argc, char **argv) {
  if (argc < 2) {
    ObjectHelp();
    return 0;
  }
  string err_msg, arg1(argv[1]);
  if (arg1 == "put") {
    return comPutObject(cdobs, argc, argv);
  }
  else {
    cout << kErrInvalidSyntax << " "
    << arg1 << endl;
    ObjectHelp();
    return 1;
  }
}

// TODO: 
// 1. Add a help message.
// 2. Case insentivity, etc. 
// 2. Use a proper parser to parse
// arguments. 
int main(int argc, char **argv) {
  dout << "Starting program cdobs" << endl; 
  if (argc < 2) {
    Help();
    exit(0);
  }

  int exit_code = 0;  
  string arg1(argv[1]);
  if (arg1 == "init") {
    exit_code = comInit();
  }
  else  {
    Cdobs *cdobs;
    int ret_value = 0;
    string err_msg;
    int rc_init = InitCdobs(&cdobs, err_msg);
    if (rc_init) {
      cout << "ERROR: " << err_msg << endl;
      exit_code = 1;
    }
    else {
      if (arg1 == "bucket") {
        // send the rest of commands
        exit_code = comBucket(cdobs, argc - 1, argv + 1);
      }
      else if (arg1 == "object") {
        exit_code = comObject(cdobs, argc - 1, argv + 1);
      }
      else {
        Help();
        exit_code = 1;
      }       
    }
  }

  exit(exit_code);
}
