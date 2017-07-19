#define MAX_QUERY_SIZE 2000 // What should this be?
#define SHORT_QUERY_SIZE 500 // What should this be?

#define STATUS_NOINIT 0
#define STATUS_GOOD 1
/* DbSotre class, wrappers over direct
 * calls to the sqllite functions
 * sanitation on user input functions to be
 * done here */
class DbStore {
private:
	static const string INSERT_BUCKET;
	static const string SELECT_ALL_BUCKETS;
	static const string SELECT_BUCKET_ID;
	static const string DELETE_BUCKET;
	static const string DELETE_DATA;
	static const string DELETE_OBJECT;

	sqlite3 *sql_db;
	int status;

	int prepare(char *stmt_str);
	int exec(char *stmt_str);
public:
	static bool check_db_init(sqlite3 *db);
	DbStore ();
	int init (char *db_name);
	int good ();
	int insert_bucket();
	int get_bucket_id(char *name);
	void empty_bucket(int bucket_id);
	int delete_bucket(int bucket_id);
	int select_all_buckets(Bucket **bucket);
	int create_object(char *name, int bucket_id, char *time);
	int create_object(char *name, int bucket_id,
		char *time, int size);
	int update_object_size(int id, int size);
}