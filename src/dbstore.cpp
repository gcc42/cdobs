#include "dbstore.h"
#include "config.h"
#include "dberror.h"
#include "cdobs.h"
#include <sqlite3.h>
#include "utils.h"

using namespace std;

const string DbStore::kCountObjects =
"SELECT Count(ObjectID) FROM ObjectDirectory";

const string DbStore::kCountBuckets =
"SELECT Count(BucketID) FROM Bucket";

// To check if the table Bucket exist
const string DbStore::kExistBucket = 
"SELECT 1 FROM Bucket LIMIT 1";

const string DbStore::kInsertBucket = 
"INSERT INTO Bucket \
 (BucketID, BucketName, Created, ObjectCount) \
 VALUES \
 (%d, '%s', '%s', %d)";

const string DbStore::kInsertObjectEntry = 
"INSERT INTO ObjectDirectory \
 (ObjectID, ObjectName, BucketID, Created, Size) \
 VALUES \
 (%d, '%s', %d, '%s', %d)";

const string DbStore::kInsertObjectData =
"INSERT INTO ObjectStore \
(ObjectID, Data) \
VALUES \
(%d, ?)";

const string DbStore::kDeleteBucket = 
"DELETE FROM Bucket \
WHERE BucketID=%d";

const string DbStore::kSelectAllBuckets = 
"SELECT * FROM Bucket";

const string DbStore::kSelectBucketId = 
"SELECT BucketID FROM Bucket \
WHERE BucketName = '%s'";

const string DbStore::kDeleteData =
"DELETE FROM ObjectStore \
WHERE ObjectID = %d";

const string DbStore::kDeleteObject =
"DELETE FROM ObjectDirectory \
WHERE ObjectID = %d";

/* Checks if the given db is initialized
 * for cdobs. Currently does this by checking 
 * if the table Bucket exists */
bool DbStore::CheckDbInit (sqlite3 *db) {
  char *err_msg;
  int rc = sqlite3_exec(db, kExistBucket.c_str(), NULL,
      NULL, &err_msg);
  if (rc != SQLITE_OK) {
    // Error Status: Table does not exist
    if (err_msg) {
      sqlite3_free(err_msg);
    }
    return false;
  }
  return true;
}

DbStore::DbStore() : sql_db_(NULL), status_(S_NOINIT) {}

int DbStore::GetObjectCount () {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
      kCountObjects.c_str());
  sqlite3_stmt *stmt = Prepare(query);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    int count = 0;
    char *count_str = (char *)sqlite3_column_text(stmt, 0);
    sscanf(count_str, "%d", &count);
    return count;
  }
  else {
    sqlite3_finalize(stmt);
    return -1;
  }
}

int DbStore::Init (const char *db_name) {
  int ret_value = 0;
  int ret = sqlite3_open(db_name, &sql_db_);
  if (ret != SQLITE_OK) {
    dout << "DB file not accessible";
    ret_value = 1;
  }
  else if (!DbStore::CheckDbInit(sql_db_)) {
    dout << "Db not initialized for Cdobs";
    ret_value = 1;
  }
  if (ret_value == 1) {
    sqlite3_close(sql_db_);
    status_ = !S_GOOD;
  }
  else {
    status_ = S_GOOD;
    object_count_ = GetObjectCount();
    if (object_count_ == -1) {
      dout << "negative object count, unexpected\n";
      ret_value = 1;
    }
  }
  return ret_value;
}

int DbStore::good () {
  return (status_ == S_GOOD);
}

// Requires a UTF-8 stmt string 
sqlite3_stmt *DbStore::Prepare (const char *stmt_str) {
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare(sql_db_, stmt_str,
      -1, &stmt, NULL);
  if (rc == SQLITE_OK) {
    return stmt;
  }
  else {
    return NULL;
  }
}

int DbStore::Exec (char *query) {
  char *err_msg = NULL;
  int rc = sqlite3_exec(sql_db_, query, NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    if (err_msg) {
      cout << err_msg;
      sqlite3_free(err_msg);          
    }
    return -1;
  }
  return 0;
}

int DbStore::GetBucketCount () {
  sqlite3_stmt *stmt = Prepare(kCountBuckets.c_str());
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    char *count_str = (char *)sqlite3_column_text(stmt, 0);
    int count = 0;
    sscanf(count_str, "%d", &count);
    return count;
  }
  else {
    return -1;
  }
}

int DbStore::InsertBucket (int bucket_id, const char *name,
  char *time, int init_count) {
  char query[MAX_QUERY_SIZE];
  int writ = snprintf(query, MAX_QUERY_SIZE,
      kInsertBucket.c_str(), bucket_id, name, time, init_count);
  if (writ < 0 || writ > MAX_QUERY_SIZE) {
    return -1;
  }
  return Exec(query); 
}

int DbStore::DeleteBucket (int bucket_id) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
      kDeleteBucket.c_str(), bucket_id);
  if (writ < 0 || writ > SHORT_QUERY_SIZE) {
    return -1;
  }
  return Exec(query);
}

int DbStore::cbListBuckets (void *data, int argc,
  char **argv, char **azColName) {
  vector<Bucket> *buckets = (vector<Bucket> *)data;
  
  int id, object_count_;
  sscanf(argv[0], "%d", &id);
  sscanf(argv[3], "%d", &object_count_);
  char *bucket_name = argv[1];
  struct tm* time = ParseTimeString(argv[2]);

  Bucket *bucket = new Bucket(id, bucket_name,
      time, object_count_);
  buckets->push_back(*bucket);
  return 0;
}

int DbStore::SelectAllBuckets (vector<Bucket> &buckets,
  string &err_msg) {
  char *err_str;
  int rc = sqlite3_exec(sql_db_, kSelectAllBuckets.c_str(),
      &DbStore::cbListBuckets, (void *)&buckets, &err_str);
  if (rc != SQLITE_OK) {
    err_msg = string(err_str);
    sqlite3_free(err_str);
    return -1;
  }
  return 0;
}

int DbStore::GetBucketId (const char *name) {
  char query[MAX_QUERY_SIZE];
  int writ = snprintf(query, MAX_QUERY_SIZE,
      kSelectBucketId.c_str(), name);
  if (writ < 0 || writ > MAX_QUERY_SIZE) {
    return -1;
  }
  sqlite3_stmt *stmt = Prepare(query);
  if (!stmt) {
    return -1;
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    char *id_str = (char *)sqlite3_column_text(stmt, 0);
    int id = -1;
    sscanf(id_str, "%d", &id);
    sqlite3_finalize(stmt);
    return id;
  }
  else {
    // Error Code: bucket not found
    return -1;
  }
}

/* Delete all objects from bucket
 * with given id 
 */
int DbStore::EmptyBucket (int id) {
  // TO be implemented. 
}

int DbStore::DeleteObject (int id) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
      kDeleteData.c_str(), id);
  // first, delete object data
  int rc = Exec(query);
  if (rc != SQLITE_OK) {
    return -1;
  }
  // Then delete the object entry
  writ = snprintf(query, SHORT_QUERY_SIZE,
      kDeleteObject.c_str(), id);
  return Exec(query);
}

int DbStore::CreateObject (const char *name, int bucket_id,
  char *time, string &err_msg) {
  // Default size, unknown before putting in the object
  return CreateObject(name, bucket_id, time, 0, err_msg);    
}

int DbStore::CreateObject (const char *name, int bucket_id,
  char *time, int size, string &err_msg) {
  char query[MAX_QUERY_SIZE];
  int obj_id = ++object_count_;
  int writ = snprintf(query, MAX_QUERY_SIZE,
      kInsertObjectEntry.c_str(), obj_id, name, 
      bucket_id, time, size);
  if (Exec(query)) {
    err_msg = "Query INSERT OBJECT ENTRY failed";
    return -1;
  }
  dout << "Create object succedded" << endl;
  return obj_id;
}

/* Insert object data into the ObjectStore table */
int DbStore::PutObject (istream &src, int id, string &err_msg) {
  if (!src.good()) {
    err_msg = "Invalid file";
    return -1;
  }
  // Get Size
  src.seekg(0, ios::end);
  streampos size = src.tellg();
  src.seekg(0);

  if (size > MAX_OBJECT_SIZE) {
    err_msg = kErrObjectTooLarge;
    return -1;
  }

  char* buffer = new char[size];
  src.read(buffer, size);

  // dout << buffer << endl;
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
      kInsertObjectData.c_str(), id);
  dout << "Put object query: " << query << endl;
  sqlite3_stmt *stmt = Prepare(query);
  if (!stmt) {
    err_msg = "Prepare failed: " + string(sqlite3_errmsg(sql_db_));
    return -1;
  }
  else {
    int rc;
    // SQLITE_STATIC because the statement is finalized
    // before the buffer is freed:
    // Column 2 for the data column
    rc = sqlite3_bind_blob(stmt, 1, buffer,
      size, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
      err_msg = "Bind failed: " + string(sqlite3_errmsg(sql_db_));
      return -1;
    } else {
      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE) {
        err_msg = "Execution failed: "
            + string(sqlite3_errmsg(sql_db_));
        return -1;
      }
    }
  }
  sqlite3_finalize(stmt);
  delete[] buffer;
  return 0;
}

int DbStore::UpdateObjectSize (int id, int size) {
  return 0;
}

DbStore::~DbStore () {
  sqlite3_close(sql_db_);
}