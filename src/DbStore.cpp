#include <DbStore.h>
#include <cdobs.h>
#include <sqlite3.h>

using namespace std;

const string DbStore::INSERT_BUCKET = "INSERT INTO Bucket \
							 (BucketID, BucketName, Created, ObjectCount) \
							 VALUES \
							 (%d, '%s', '%s', %d)";

const string DbStore::DELETE_BUCKET = "DELETE FROM Bucket \
							 WHERE BucketID=%d";

const string DbStore::SELECT_BUCKETS = "SELECT * FROM Bucket";

DbStore::DbStore() : sql_db(NULL) {}

int DbStore::init(char *db_name) {
	int ret = sqlite3_open(db_name, &sql_db);
	if (ret) {
		sqlite3_close(sql_db);
		status = !GOOD;
		return 1;
	}

}

int DbStore::good () {
	return (status == STATUS_GOOD);
}

// Requires a UTF-8 stmt string 
void DbStore::prepare (char *stmt_str) {
	sqlite3_stmt *stmt;
	sqlite3_prepare(sql_db, stmt_str, -1, &stmt, NULL);
	return stmt;
}

int DbStore::exec (char *query) {
	char *err_msg = NULL;
	int rc = sqlite3_exec(sql_db, query, NULL, NULL, &err_msgr);
	if (rc != SQLITE_OK && err_msg) {
		cout << err_msg;
		sqlite3_free(err_msg);
		return -1;
	}
	return 0;
}

int DbStore::insert_bucket (int bucket_id, char *name, char *time, 
	int init_count) {
	char query[MAX_QUERY_SIZE];
	int writ = snprintf(query, MAX_QUERY_SIZE,
		INSERT_BUCKET, bucket_id, name, time, init_count);
	if (writ < 0 || writ > MAX_QUERY_SIZE) {
		return -1;
	}
	return exec(query);	
}

int DbStore::delete_bucket (int bucket_id) {
	char query[SHORT_QUERY_SIZE];
	int writ = snprintf(query, SHORT_QUERY_SIZE,
		DELETE_BUCKET, id);
	if (writ < 0 || writ > SHORT_QUERY_SIZE) {
		return -1;
	}
	return exec(query);
}

int DbStore::list_bucket_callback (void *data, 
	int argc, char **argv, char **azColName) {
	vector<Bucket> *buckets = (vector<Bucket> *)data;
	
	int id, object_count;
	sscanf(argv[0], "%d", &id);
	sscanf(argv[3], "%d", &object_count);
	char *bucket_name = argc[1];
	struct tm* time = parse_time_string(argv[2]);

	Bucket *bucket = new Bucket(id, bucket_name,
		time, object_count);
	buckets->push_back(*bucket);
	return 0;
}

int DbStore::select_all_buckets (vector<Bucket> **buckets) {
	*buckets = new vector<Bucket>();
	char *err_msg;
	int rc = sqlite3_exec(sql_db, SELECT_BUCKETS, list_bucket_callback,
		(void *)*buckets, err_msg);
	if (rc != SQLITE_OK) {
		return -1;
	}
	return 0;
}