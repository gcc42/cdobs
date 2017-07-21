#include "dbstore.h"
#include "config.h"
#include "dberror.h"
#include "cdobs.h"
#include <sqlite3.h>
#include "utils.h"

using namespace std;

const string DbStore::COUNT_OBJECTS =
"SELECT Count(ObjectID) FROM ObjectDirectory";

const string DbStore::COUNT_BUCKETS =
"SELECT Count(BucketID) FROM Bucket";

// To check if the table Bucket exist
const string DbStore::EXIST_BUCKET = 
"SELECT 1 FROM Bucket LIMIT 1";

const string DbStore::INSERT_BUCKET = 
"INSERT INTO Bucket \
 (BucketID, BucketName, Created, ObjectCount) \
 VALUES \
 (%d, '%s', '%s', %d)";

const string DbStore::INSERT_OBJECT_ENTRY = 
"INSERT INTO ObjectDirectory \
 (ObjectID, ObjectName, BucketID, Created, Size) \
 VALUES \
 (%d, '%s', %d, '%s', %d)";

const string DbStore::INSERT_OBJECT_DATA =
"INSERT INTO ObjectStore \
(ObjectID, Data) \
VALUES \
(%d, ?)";

const string DbStore::DELETE_BUCKET = 
"DELETE FROM Bucket \
WHERE BucketID=%d";

const string DbStore::SELECT_ALL_BUCKETS = 
"SELECT * FROM Bucket";

const string DbStore::SELECT_BUCKET_ID = 
"SELECT BucketID FROM Bucket \
WHERE BucketName = '%s'";

const string DbStore::DELETE_DATA =
"DELETE FROM ObjectStore \
WHERE ObjectID = %d";

const string DbStore::DELETE_OBJECT =
"DELETE FROM ObjectDirectory \
WHERE ObjectID = %d";

/* Checks if the given db is initialized
 * for cdobs. Currently does this by checking 
 * if the table Bucket exists */
bool DbStore::check_db_init (sqlite3 *db) {
	char *err_msg;
	int rc = sqlite3_exec(db, EXIST_BUCKET.c_str(), NULL,
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

DbStore::DbStore() : sql_db(NULL), status(S_NOINIT) {}

int DbStore::get_object_count () {
	char query[SHORT_QUERY_SIZE];
	int writ = snprintf(query, SHORT_QUERY_SIZE,
		COUNT_OBJECTS.c_str());
	sqlite3_stmt *stmt = prepare(query);
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

int DbStore::init(char *db_name) {
	int retValue = 0;
	int ret = sqlite3_open(db_name, &sql_db);
	if (ret != SQLITE_OK) {
		dout << "DB file not accessible";
		retValue = 1;
	}
	else if (!DbStore::check_db_init(sql_db)) {
		dout << "Db not initialized for Cdobs";
		retValue = 1;
	}
	if (retValue == 1) {
		sqlite3_close(sql_db);
		status = !S_GOOD;
	}
	else {
		status = S_GOOD;
		object_count = get_object_count();
		if (object_count == -1) {
			dout << "negative object count, unexpected\n";
			retValue = 1;
		}
	}
	return retValue;
}

int DbStore::good () {
	return (status == S_GOOD);
}

// Requires a UTF-8 stmt string 
sqlite3_stmt *DbStore::prepare (const char *stmt_str) {
	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare(sql_db, stmt_str,
		-1, &stmt, NULL);
	if (rc == SQLITE_OK) {
		return stmt;
	}
	else {
		return NULL;
	}
}

int DbStore::exec (char *query) {
	char *err_msg = NULL;
	int rc = sqlite3_exec(sql_db, query, NULL, NULL, &err_msg);
	if (rc != SQLITE_OK) {
		if (err_msg) {
			cout << err_msg;
			sqlite3_free(err_msg);			
		}
		return -1;
	}
	return 0;
}

int DbStore::get_bucket_count () {
	sqlite3_stmt *stmt = prepare(DbStore::COUNT_BUCKETS.c_str());
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

int DbStore::insert_bucket (int bucket_id, const char *name,
	char *time, int init_count) {
	char query[MAX_QUERY_SIZE];
	int writ = snprintf(query, MAX_QUERY_SIZE,
		INSERT_BUCKET.c_str(), bucket_id, name, time, init_count);
	if (writ < 0 || writ > MAX_QUERY_SIZE) {
		return -1;
	}
	return exec(query);	
}

int DbStore::delete_bucket (int bucket_id) {
	char query[SHORT_QUERY_SIZE];
	int writ = snprintf(query, SHORT_QUERY_SIZE,
		DELETE_BUCKET.c_str(), bucket_id);
	if (writ < 0 || writ > SHORT_QUERY_SIZE) {
		return -1;
	}
	return exec(query);
}

int DbStore::cb_list_buckets (void *data, 
	int argc, char **argv, char **azColName) {
	vector<Bucket> *buckets = (vector<Bucket> *)data;
	
	int id, object_count;
	sscanf(argv[0], "%d", &id);
	sscanf(argv[3], "%d", &object_count);
	char *bucket_name = argv[1];
	struct tm* time = parse_time_string(argv[2]);

	Bucket *bucket = new Bucket(id, bucket_name,
		time, object_count);
	buckets->push_back(*bucket);
	return 0;
}

int DbStore::select_all_buckets (vector<Bucket> &buckets,
	string &err_msg) {
	char *err_str;
	int rc = sqlite3_exec(sql_db, SELECT_ALL_BUCKETS.c_str(),
		&DbStore::cb_list_buckets, (void *)&buckets, &err_str);
	if (rc != SQLITE_OK) {
		err_msg = string(err_str);
		sqlite3_free(err_str);
		return -1;
	}
	return 0;
}

int DbStore::get_bucket_id (const char *name) {
	char query[MAX_QUERY_SIZE];
	int writ = snprintf(query, MAX_QUERY_SIZE,
		SELECT_BUCKET_ID.c_str(), name);
	if (writ < 0 || writ > MAX_QUERY_SIZE) {
		return -1;
	}
	sqlite3_stmt *stmt = prepare(query);
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
int DbStore::empty_bucket (int id) {
	// TO be implemented. 
}

int DbStore::delete_object (int id) {
	char query[SHORT_QUERY_SIZE];
	int writ = snprintf(query, SHORT_QUERY_SIZE,
		DELETE_DATA.c_str(), id);
	// first, delete object data
	int rc = exec(query);
	if (rc != SQLITE_OK) {
		return -1;
	}
	// Then delete the object entry
	writ = snprintf(query, SHORT_QUERY_SIZE,
		DELETE_OBJECT.c_str(), id);
	return exec(query);
}

int DbStore::create_object (const char *name, int bucket_id,
	char *time, string &err_msg) {
	// Default size, unknown before putting in the object
	return create_object(name, bucket_id, time, 0, err_msg);	
}

int DbStore::create_object (const char *name, int bucket_id,
	char *time, int size, string &err_msg) {
	char query[MAX_QUERY_SIZE];
	int obj_id = ++object_count;
	int writ = snprintf(query, MAX_QUERY_SIZE,
		INSERT_OBJECT_ENTRY.c_str(), obj_id, name, 
		bucket_id, time, size);
	if (exec(query)) {
		err_msg = "Query INSERT_OBJECT_ENTRY failed";
		return -1;
	}
	return 0;
}

/* Insert object data into the ObjectStore table */
int DbStore::put_object (istream &src, int id, string &err_msg) {
	if (!src.good()) {
		err_msg = "Invalid file";
		return -1;
	}
    // Get Size
    src.seekg(0, ios::end);
    streampos size = src.tellg();
    src.seekg(0);

    if (size > MAX_OBJECT_SIZE) {
    	err_msg = ERR_OBJECT_TOO_LARGE;
    	return -1;
    }

    char* buffer = new char[size];
    src.read(buffer, size);

    char query[SHORT_QUERY_SIZE];
    int writ = snprintf(query, SHORT_QUERY_SIZE,
    	INSERT_OBJECT_DATA.c_str(), id);
    sqlite3_stmt *stmt = prepare(query);
    if (!stmt) {
       err_msg = "Prepare failed: " + string(sqlite3_errmsg(sql_db));
       return -1;
    } else {
    	int rc;
        // SQLITE_STATIC because the statement is finalized
        // before the buffer is freed:
        // Column 2 for the data column
        rc = sqlite3_bind_blob(stmt, 2, buffer,
        	size, SQLITE_STATIC);
        if (rc != SQLITE_OK) {
            err_msg = "Bind failed: " + string(sqlite3_errmsg(sql_db));
            return -1;
        } else {
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
            	err_msg = "Execution failed: "
            		+ string(sqlite3_errmsg(sql_db));
            	return -1;
            }
        }
    }
    sqlite3_finalize(stmt);
    delete[] buffer;
}

int DbStore::update_object_size (int id, int size) {
	return 0;
}