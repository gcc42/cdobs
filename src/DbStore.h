#define MAX_QUERY_SIZE 2000 // What should this be?

/* DbSotre class, wrappers over direct
 * calls to the sqllite functions
 * sanitation on user input functions to be
 * done here */
class DbStore {
private:
	static const string INSERT_BUCKET;
	static const string SELECT_ALL_BUCKETS;
	static const string DELETE_BUCKET;
	sqlite3 *sql_db;
	int status;
	void prepare(char *stmt_str);
public:
	DbStore ();
	int init (char *db_name);
	int good ();
	int insert_bucket();
	int get_bucket_id_from_name(char *name);
	void delete_all_objects_from_bucket(int bucket_id);
	int delete_bucket(int bucket_id);
	int select_all_buckets(Bucket **bucket);
	int create_object(char *name, int bucket_id, char *time);
	int create_object(char *name, int bucket_id,
		char *time, int size);
	int update_object_size(int id, int size);
}