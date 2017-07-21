#include "create.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <sqlite3.h>
#include "config.h"
#include "utils.h"
#include "dberror.h"
#include "dbstore.h"

using namespace std;

// path must be a valid fully qualified path
static int create_db (string path, string file_name) {
	ofstream db_file(path + file_name, ios::binary);
	if (!db_file.good())
		return -1;
	return 0;
}

static bool file_exists (string path, string file_name) {
	ifstream file(path + file_name);
	int ret = file.good();
	file.close();
	return ret;
}

static int setup_tables (string path, string file_name) {
	sqlite3 *db;
	sqlite3_open((path + file_name).c_str(), &db);
	if (DbStore::check_db_init(db)) {
		return 1;
	}

	char buf[MAX_QUERY_SIZE];

	// Create Bucket table
	snprintf(buf, MAX_QUERY_SIZE,
		SQL_CREATE.c_str(), BUCKET_SCHEMA.c_str());
	dout << buf;
	if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
		return FAILURE;  	
	}

	// Create object dirrectory table
	snprintf(buf, MAX_QUERY_SIZE,
		SQL_CREATE.c_str(), OBJECT_DIR_SCHEMA.c_str());
	if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
		return FAILURE;  	
	}

	// Create object store table
	snprintf(buf, MAX_QUERY_SIZE,
		SQL_CREATE.c_str(), OBJECT_STORE_SCHEMA.c_str());
	if (sqlite3_exec(db, buf, NULL, NULL, NULL) != SQLITE_OK) {
		return FAILURE;  	
	}

	return SUCCESS;
}

int setup_database (string &err_msg) {
	if (!file_exists(DB_PATH, SQLITE_DB_FILE)) {
		int rc_create = create_db(DB_PATH, SQLITE_DB_FILE);
		if (rc_create) {
			err_msg = ERR_CANT_CREATE_FILE;
			return -1;
		}
	}

	int rc_setup = setup_tables(DB_PATH, SQLITE_DB_FILE);
	if (rc_setup) {
		err_msg = ERR_CANT_SETUP_TABLE;
		return -1;
	} 
	
	return 0;
}
