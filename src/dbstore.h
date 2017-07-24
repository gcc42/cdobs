#ifndef DBSTORE_H
#define DBSTORE_H

#define MAX_QUERY_SIZE 2000
#define SHORT_QUERY_SIZE 500

#define S_NOINIT -1
#define S_GOOD 1

#include <vector>
#include <string>
#include <sqlite3.h>

struct Bucket {
  int id;
  std::string name;
  struct tm *created;
  int object_count;
  
  Bucket(int p_id, char *p_name,  struct tm* time,
        int count): id(p_id), name(p_name), created(time),
      object_count(count) {}

  Bucket() {}
};

struct Object {
  int id;
  std::string name;
  int bucket_id;
  struct tm *created;
  int size;

  Object(int p_id, char *p_name, int p_bucketid,
        struct tm *p_created, int p_size): id(p_id),
      name(p_name), bucket_id(p_bucketid), created(p_created),
      size(p_size) {}

  Object() {}
};

/* DbStore class, wrappers over direct
 * calls to the sqlite functions
 * sanitation on user input functions to be
 * done here */
class DbStore {

private:
  static const std::string kCountObjects;
  static const std::string kCountBuckets;
  static const std::string kExistBucket;
  static const std::string kInsertBucket;
  static const std::string kInsertObjectEntry;
  static const std::string kInsertObjectData;
  static const std::string kSelectAllBuckets;
  static const std::string kSelectBucketId;
  static const std::string kSelectObjectId;
  static const std::string kDeleteBucket;
  static const std::string kDeleteData;
  static const std::string kDeleteObject;
  static const std::string kSelectObjectsInBucket;
  static const std::string kUpdateObjectSize;

  sqlite3 *sql_db_;
  int status_;
  int object_count_;

  sqlite3_stmt *Prepare(const char *stmt_str);
  int Exec(char *stmt_str);
  int ExecSingleValueQuery (const char *query, int *value);

public:
  static bool CheckDbInit(sqlite3 *db);
  DbStore ();
  ~DbStore ();
  int Init (const char *db_name);
  int good ();
  int InsertBucket(int bucket_id, const char *name, char *time, 
                  int init_count);
  int GetBucketId(const char *name);
  int GetObjectId(int bucket_id, const char *name);
  int GetBucketCount();
  int GetObjectCount();
  int EmptyBucket(int bucket_id);
  int DeleteBucket(int bucket_id);
  static int cbListBuckets (void *data, int argc,
                            char **argv, char **azColName);
  int SelectAllBuckets(std::vector<Bucket> &buckets,
                      std::string &err_msg);
  int DeleteObjectEntry(int id);
  int DeleteObjectData(int id);
  int CreateObjectEntry(const char *name, int bucket_id,
                        char *time, std::string &err_msg);
  int CreateObjectEntry(const char *name, int bucket_id,
                        char *time, int size, std::string &err_msg);
  int UpdateObjectSize(int id, int size);
  int PutObjectData(std::istream &src, int id, std::string &err_msg);
  static int cbSelectObjects(void *data, int argc,
                            char **argv, char **azColName);
  int SelectObjectsInBucket(int bucket_id, std::vector<Object> &objects,
                    std::string &err_msg);
};

#endif
