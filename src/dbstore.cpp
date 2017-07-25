#include "dbstore.h"
#include "config.h"
#include "dberror.h"
#include "cdobs.h"
#include <sqlite3.h>
#include "utils.h"

using namespace std;

const string DbStore::kEnableForeignKey =
"PRAGMA foreign_keys = ON";

const string DbStore::kBeginTrans =
"BEGIN TRANSACTION";

const string DbStore::kCommitTrans =
"COMMIT TRANSACTION";

const string DbStore::kRollbackTrans =
"ROLLBACK TRANSACTION";

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
 (ObjectID, ObjectName, BucketID, Created, Size, Lob) \
 VALUES (%d, '%s', %d, '%s', %d, %d)";

const string DbStore::kInsertObjectData =
"INSERT INTO ObjectStore \
(ObjectID, Data) \
VALUES (%d, ?)";

const string DbStore::kInsertLobData =
"INSERT INTO LargeObjectStore \
(ObjectID, Segment, SegmentSize, Data) \
VALUES (%d, %d, %d, ?)";

const string DbStore::kDeleteBucket = 
"DELETE FROM Bucket \
WHERE BucketID=%d";

const string DbStore::kSelectAllBuckets = 
"SELECT * FROM Bucket";

const string DbStore::kSelectBucketId = 
"SELECT BucketID FROM Bucket \
WHERE BucketName = '%s'";

const string DbStore::kSelectObjectId = 
"SELECT ObjectID FROM ObjectDirectory \
WHERE ObjectName = '%s' and BucketID = %d";

const string DbStore::kDeleteData =
"DELETE FROM ObjectStore \
WHERE ObjectID = %d";

const string DbStore::kDeleteObject =
"DELETE FROM ObjectDirectory \
WHERE ObjectID = %d";

const string DbStore::kSelectObjectsInBucket =
"SELECT * FROM ObjectDirectory \
WHERE BucketID = %d";

const string DbStore::kUpdateObjectSize =
"UPDATE ObjectDirectory \
SET Size=%d \
WHERE ObjectID=%d";

const string DbStore::kSelectObjectIds =
"SELECT ObjectID FROM ObjectDirectory \
ORDER BY ObjectID ASC";

const string DbStore::kSelectObjectIdsInBucket =
"SELECT ObjectID FROM ObjectDirectory \
WHERE BucketID=%d \
ORDER BY ObjectID ASC";

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
  else {
    Exec(kEnableForeignKey.c_str());
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

int DbStore::BeginTransaction() {
  return Exec(kBeginTrans.c_str());
}

int DbStore::CommitTransaction() {
  return Exec(kCommitTrans.c_str());
}

int DbStore::RollbackTransaction() {
  return Exec(kRollbackTrans.c_str());
}

// Requires a UTF-8 stmt string 
sqlite3_stmt *DbStore::Prepare(const char *stmt_str) {
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

int DbStore::Exec(const char *query) {
  char *err_msg = NULL;
  int rc = sqlite3_exec(sql_db_, query, NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    if (err_msg) {
      dout << err_msg;
      sqlite3_free(err_msg);          
    }
    return -1;
  }
  return 0;
}

/* Executes query and returns a single
 * integer value that it returns */
int DbStore::ExecValueQuery(const char *query, int *value) {
  sqlite3_stmt *stmt = Prepare(query);
  if (!stmt) {
    return -1;
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    char *result_str = (char *)sqlite3_column_text(stmt, 0);
    *value = -1;
    sscanf(result_str, "%d", value);
    sqlite3_finalize(stmt);
    return 0;
  }
  else {
    return -1;
  }
}

int DbStore::ExecVectorQuery(const char *query, vector<int> &values) {
  sqlite3_stmt *stmt = Prepare(query);
  if (!stmt) {
    return -1;
  }

  char *result_str; 
  int value, rc_step;
  while ((rc_step = sqlite3_step(stmt)) == SQLITE_ROW) {
    result_str = (char *)sqlite3_column_text(stmt, 0);
    sscanf(result_str, "%d", &value);
    values.push_back(value);
  }

  sqlite3_finalize(stmt);
  return !(rc_step == SQLITE_DONE);
}

int DbStore::GetBucketCount() {
  int count;
  if (!ExecValueQuery(kCountBuckets.c_str(), &count)) {
    return count;
  }
  else {
    return -1;
  }
}

int DbStore::GetObjectCount() {
  int count;
  if (!ExecValueQuery(kCountObjects.c_str(), &count)) {
    return count;
  }
  else {
    return -1;
  }
}

int DbStore::InsertBucket(int bucket_id, const char *name,
                          char *time, int init_count) {
  char query[MAX_QUERY_SIZE];
  int writ = snprintf(query, MAX_QUERY_SIZE,
      kInsertBucket.c_str(), bucket_id, name, time, init_count);
  if (writ < 0 || writ > MAX_QUERY_SIZE) {
    return -1;
  }
  return Exec(query); 
}

int DbStore::DeleteBucketEntry(const int bucket_id) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
      kDeleteBucket.c_str(), bucket_id);
  if (writ < 0 || writ > SHORT_QUERY_SIZE) {
    return -1;
  }
  return Exec(query);
}

int DbStore::cbListBuckets(void *data, int argc,
                          char **argv, char **azColName) {
  vector<Bucket> *buckets = (vector<Bucket> *)data;
  
  int id, object_count;
  sscanf(argv[0], "%d", &id);
  sscanf(argv[3], "%d", &object_count);
  char *bucket_name = argv[1];
  struct tm* time = ParseTimeString(argv[2]);

  Bucket bucket(id, bucket_name,
                time, object_count);
  buckets->push_back(bucket);
  return 0;
}

int DbStore::SelectAllBuckets(vector<Bucket> &buckets, string &err_msg) {
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

int DbStore::SelectAllObjectIds(vector<int> &object_ids) {
  return ExecVectorQuery(kSelectObjectIds.c_str(), object_ids);
}

int DbStore::GetBucketId(const char *name) {
  char query[MAX_QUERY_SIZE];
  int writ = snprintf(query, MAX_QUERY_SIZE,
                      kSelectBucketId.c_str(), name);
  if (writ < 0 || writ > MAX_QUERY_SIZE) {
    return -1;
  }
  int id;
  if (ExecValueQuery(query, &id)) {
    return -1;
  }
  else {
    return id;
  }
}

int DbStore::GetObjectId(int bucket_id, const char *name) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE, kSelectObjectId.c_str(), 
                      name, bucket_id);
  if (writ < 0 || writ > MAX_QUERY_SIZE) {
    return -1;
  }
  int id = -1;
  if (ExecValueQuery(query, &id)) {
    return -1;
  }
  else {
    return id;
  }
} 

int DbStore::SelectObjectIdsInBucket(
    const int bucket_id, vector<int> &object_ids) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE, 
                      kSelectObjectsInBucket.c_str(), bucket_id);
  return ExecVectorQuery(query, object_ids);
}

/* Delete the object data from ObjectStore
 */
int DbStore::DeleteObjectData(int id) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
                      kDeleteData.c_str(), id);
  return Exec(query);
}

/* Delete the object entry from ObjectDirectory
 */
int DbStore::DeleteObjectEntry(int id) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
                      kDeleteObject.c_str(), id);
  return Exec(query);
}

int DbStore::CreateObjectEntry(
    const int id, const char *name, const int bucket_id,
    char *time, int lob_flag string &err_msg) {
  // Default size, unknown before putting in the object data
  return CreateObjectEntry(id, name, bucket_id, time, 0, lob_flag, err_msg);    
}

int DbStore::CreateObjectEntry(
    const int id, const char *name, const int bucket_id,
    char *time, int size, int lob_flag, string &err_msg) {
  char query[MAX_QUERY_SIZE];
  int writ = snprintf(query, MAX_QUERY_SIZE,
                      kInsertObjectEntry.c_str(), id, name, 
                      bucket_id, time, size, lob_flag);
  if (Exec(query)) {
    err_msg = "Query INSERT OBJECT ENTRY failed";
    return -1;
  }
  dout << "Create object succedded" << endl;
  return 0;
}

/* Insert size bytes of binary data into
 * ObjectStore or LargeObjectStore depending on
 * whether segment = -1 or segment > 0
 */
int DbStore::PutObjectData(const int id, istream &src, int size,
                           int segment, string &err_msg) {
  char* buffer = new char[size];
  src.read(buffer, size);
  int size_read = src.gcount();

  char query[SHORT_QUERY_SIZE];
  int writ;
  if (segment > 0) {
    snprintf(query, SHORT_QUERY_SIZE, kInsertObjectData.c_str(), id);
  }
  else {
    snprintf(query, SHORT_QUERY_SIZE, kInsertLobData.c_str(), id,
            segment, size_read);
  }
  dout << "Put object query: " << query << endl;
  sqlite3_stmt *stmt = Prepare(query);
  if (!stmt) {
    err_msg = "Prepare failed: " + string(sqlite3_errmsg(sql_db_));
    size_read = -1;
  }
  else {
    int rc;
    rc = sqlite3_bind_blob(stmt, 1, buffer, size_read, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
      err_msg = "Bind failed: " + string(sqlite3_errmsg(sql_db_));
      size_read = -1;
    } else {
      rc = sqlite3_step(stmt);
      if (rc != SQLITE_DONE) {
        err_msg = "Execution failed: "
            + string(sqlite3_errmsg(sql_db_));
        size_read = -1;
      }
    }
  }
  sqlite3_finalize(stmt);
  delete[] buffer;
  return size_read;
}

/* Insert object data into the ObjectStore table */
int DbStore::PutObjectData(const int id, istream &src, string &err_msg) {
  if (!src.good()) {
    err_msg = "Invalid file";
    return -1;
  }
  // Get Size
  src.seekg(0, ios::end);
  streampos size = src.tellg();
  src.seekg(0);

  if (size > kMaxObjectSize) {
    err_msg = kErrObjectTooLarge;
    return -1;
  }
  return PutObjectData(id, src, size, -1, err_msg);
}

int DbStore::UpdateObjectSize(int id, int size) {
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE,
                      kUpdateObjectSize.c_str(), size, id);
  return Exec(query);
}

int DbStore::cbSelectObjects(void *data, int argc,
                            char **argv, char **azColName) {
  vector<Object> *objects = (vector<Object> *)data;
  int id, bucket_id, size;
  sscanf(argv[0], "%d", &id);
  sscanf(argv[2], "%d", &bucket_id);
  sscanf(argv[4], "%d", &size);
  char *object_name = argv[1];
  struct tm* time = ParseTimeString(argv[3]);

  Object object(id, object_name, bucket_id,
                time, size);
  objects->push_back(object);
  return 0;
}

int DbStore::SelectObjectsInBucket(
    int bucket_id, std::vector<Object> &objects, std::string &err_msg) {
  char *err_str;
  char query[SHORT_QUERY_SIZE];
  int writ = snprintf(query, SHORT_QUERY_SIZE, 
                      kSelectObjectsInBucket.c_str(), bucket_id);
  int rc = sqlite3_exec(sql_db_, query, &DbStore::cbSelectObjects,
                        (void *)&objects, &err_str);
  if (rc != SQLITE_OK) {
    err_msg = string(err_str);
    sqlite3_free(err_str);
    return -1;
  }
  return 0;
}

DbStore::~DbStore() {
  sqlite3_close(sql_db_);
}