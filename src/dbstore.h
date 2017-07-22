#ifndef DBSTORE_H
#define DBSTORE_H

#define MAX_QUERY_SIZE 2000
#define SHORT_QUERY_SIZE 500
#define MAX_OBJECT_SIZE 5000000 // 5 mb

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

  Bucket () {}
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
  static const std::string kDeleteBucket;
  static const std::string kDeleteData;
  static const std::string kDeleteObject;

  sqlite3 *sql_db_;
  int status_;
  int object_count_;

  sqlite3_stmt *Prepare(const char *stmt_str);
  int Exec(char *stmt_str);
public:
  static bool CheckDbInit(sqlite3 *db);
  DbStore ();
  ~DbStore ();
  int Init (char *db_name);
  int good ();
  int InsertBucket(int bucket_id, const char *name, char *time, 
    int init_count);
  int GetBucketId(const char *name);
  int GetBucketCount();
  int GetObjectCount();
  int EmptyBucket (int bucket_id);
  int DeleteBucket (int bucket_id);
  static int cbListBuckets (void *data, int argc,
    char **argv, char **azColName);
  int SelectAllBuckets (std::vector<Bucket> &buckets,
    std::string &err_msg);
  int DeleteObject (int id);
  int CreateObject (const char *name, int bucket_id,\
    char *time, std::string &err_msg);
  int CreateObject (const char *name, int bucket_id,
    char *time, int size, std::string &err_msg);
  int UpdateObjectSize (int id, int size);
  int PutObject (std::istream &src, int id, std::string &err_msg);
};

#endif
