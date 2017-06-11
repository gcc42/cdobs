#include <create.h>
#include <config.h>
#include <string>
#include <cstdio>
#include <fstream>
#include <sqlite3.h>
#include <utils.h>

using namespace std;

int setup_database () {
  int ret = 1;
  // Create databse file and setup tables
  if (create_db(DB_PATH)) {
      ret &= setup_tables();
  }
  return ret;
}

// path must be a valid fully qualified path
int create_db (string path, string file_name) {
  ofstream db_file(path + file_name, ios::binary);
  if (!db_file.good())
    return FAILURE;
  return SUCCESS;
}

bool file_exists (string file_name) {
  ifstream file(file_name);
  return f.good();
}

int setup_tables () {
  sqlite3 *db;
  sqlite3_open(path + file_name);
  char buf[MAX_QUERY_SIZE];

  // Create Bucket table
  snprintf(buf, MAX_QUERY_SIZE, sql_create, bucket_schema);
  if (!sql_execute(db, buf))
    return FAILURE;

  // Create object dirrectory table
  snprintf(buf, MAX_QUERY_SIZE, sql_create, object_directory_schema);
  if (!sql_execute(db, buf))
    return FAILURE;

  // Create object store table
  snprintf(buf, MAX_QUERY_SIZE, sql_create, object_store_schema);
  if (!sql_execute(db, buf))
    return FAILURE;

  return SUCCESS;
}