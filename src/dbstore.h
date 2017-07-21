#ifndef DBSTORE_H
#define DBSTORE_H

#define MAX_QUERY_SIZE 2000 // What should this be?
#define SHORT_QUERY_SIZE 500 // What should this be?
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



/* DbSotre class, wrappers over direct
 * calls to the sqllite functions
 * sanitation on user input functions to be
 * done here */
class DbStore {
private:
	static const std::string COUNT_OBJECTS;
	static const std::string COUNT_BUCKETS;
	static const std::string EXIST_BUCKET;
	static const std::string INSERT_BUCKET;
	static const std::string INSERT_OBJECT_ENTRY;
	static const std::string INSERT_OBJECT_DATA;
	static const std::string SELECT_ALL_BUCKETS;
	static const std::string SELECT_BUCKET_ID;
	static const std::string DELETE_BUCKET;
	static const std::string DELETE_DATA;
	static const std::string DELETE_OBJECT;

	sqlite3 *sql_db;
	int status;
	int object_count;
	sqlite3_stmt *prepare(const char *stmt_str);
	int exec(char *stmt_str);
public:
	static bool check_db_init(sqlite3 *db);
	DbStore ();
	~DbStore ();
	int init (char *db_name);
	int good ();
	int insert_bucket(int bucket_id, const char *name, char *time, 
		int init_count);
	int get_bucket_id(const char *name);
	int get_bucket_count();
	int get_object_count();
	int empty_bucket(int bucket_id);
	int delete_bucket(int bucket_id);
	static int cb_list_buckets (void *data, int argc,
		char **argv, char **azColName);
	int select_all_buckets(std::vector<Bucket> &buckets,
		std::string &err_msg);
	int delete_object (int id);
	int create_object(const char *name, int bucket_id,\
		char *time, std::string &err_msg);
	int create_object(const char *name, int bucket_id,
		char *time, int size, std::string &err_msg);
	int update_object_size(int id, int size);
	int put_object(std::istream &src, int id, std::string &err_msg);
};

#endif
