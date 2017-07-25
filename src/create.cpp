#include "create.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <sqlite3.h>
#include "config.h"
#include "utils.h"
#include "dberror.h"
#include "dbstore.h"

using namespace std;

const std::string SQL_CREATE = "CREATE TABLE %s;";

const std::string kBucketSchema = 
"Bucket ( \
  BucketID INTEGER PRIMARY KEY, \
  BucketName VARCHAR UNIQUE, \
  Created DATE, \
  ObjectCount INTEGER \
)";

const std::string kObjectDirSchema =
"ObjectDirectory ( \
  ObjectID INTEGER PRIMARY KEY, \
  ObjectName TEXT, \
  BucketID INTEGER, \
  Created DATE, \
  Size INTEGER, \
  Lob BOOLEAN, \
  FOREIGN KEY(BucketID) REFERENCES Bucket(BucketID) \
)";

const std::string kObjectStoreSchema = 
"ObjectStore ( \
  ObjectID INTEGER REFERENCES ObjectDirectory, \
  Data BLOB, \
  PRIMARY KEY(ObjectID) \
)";

const std::string kLobStoreSchema = 
"LargeObjectStore (
  ObjectID INTEGER FOREIGN KEY REFERENCES ObjectDirectory, \
  Segment Integer, \
  Size of Segment INTEGER, \
  Data BLOB, \
  PRIMARY KEY(ObjectID, Segment) \
)";

// path must be a valid fully qualified path
static int CreateDb (const string &file_name) {
  ofstream db_file(file_name, ios::binary);
  if (!db_file.good())
    return -1;
  return 0;
}

static bool FileExists (const string &file_name) {
  ifstream file(file_name);
  int ret = file.good();
  file.close();
  return ret;
}

static int SetupTables (const string &file_name) {
  sqlite3 *db;
  sqlite3_open(file_name.c_str(), &db);
  if (DbStore::CheckDbInit(db)) {
    return 1;
  }

  char buf[MAX_QUERY_SIZE];

  // Create Bucket table
  snprintf(buf, MAX_QUERY_SIZE, SQL_CREATE.c_str(),
          kBucketSchema.c_str());
  dout << buf;
  if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
    return FAILURE;   
  }

  // Create object dirrectory table
  snprintf(buf, MAX_QUERY_SIZE, SQL_CREATE.c_str(),
          kObjectDirSchema.c_str());
  if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
    return FAILURE;   
  }

  // Create object store table
  snprintf(buf, MAX_QUERY_SIZE, SQL_CREATE.c_str(),
          kObjectStoreSchema.c_str());
  if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
    return FAILURE;   
  }

  // Create object store table
  snprintf(buf, MAX_QUERY_SIZE, SQL_CREATE.c_str(),
          kLobStoreSchema.c_str());
  if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
    return FAILURE;   
  }

  return SUCCESS;
}

int SetupDatabase (const string &db_file, string &err_msg) {
  if (!FileExists(db_file)) {
    int rc_create = CreateDb(db_file);
    if (rc_create) {
      err_msg = kErrCantCreateFile;
      return -1;
    }
  }

  int rc_setup = SetupTables(db_file);
  if (rc_setup) {
    err_msg = kErrCantSetupTable;
    return -1;
  } 
  
  return 0;
}
