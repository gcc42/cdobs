#include "cdobs.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>
#include "config.h"
#include "utils.h"
#include "dbstore.h"
#include "dberror.h"

using namespace std;

#ifdef IS_DEBUG
ostream &dout = cout;
#else
std::ofstream devnull_file(DEV_NULL);
ostream &dout = devnull_file;
#endif 

Cdobs::Cdobs (DbStore *store): state(S_NOINIT) {
	this->store = store;
	state = store->good() ? S_GOOD : !S_GOOD;
	bucket_count = store->get_bucket_count();
}

int Cdobs::good () {
	return (state == S_GOOD);
}

int Cdobs::create_bucket (string name, string &err_msg) {
	char ctime[MAX_TIME_LENGTH];
	// Get time as an "YYYY-MM-DD HH:MM:SS" format string
	int writ = get_current_time(ctime, MAX_TIME_LENGTH);
	int bucket_id = ++bucket_count;
	int intial_obj_count = 0;

	const char *bname = name.c_str();
	// Check if bucket name already exists
	if (store->get_bucket_id(bname) != -1) {
		err_msg = ERR_BUCKET_ALREADY_EXIST;
		return 1;
	}

	int err = store->insert_bucket(bucket_id, bname, ctime,
		intial_obj_count);
	return err;
}

// Also needs to delete all objects first.
int Cdobs::delete_bucket (string name) {
	int id = store->get_bucket_id(name.c_str());
	if (id == -1) {
		return 1;
	}
	store->empty_bucket(id);
	int err = store->delete_bucket(id);
	return err;
}

int Cdobs::list_buckets (vector<Bucket> &buckets,
	string &err_msg) {
	int count = store->select_all_buckets(buckets, err_msg);
	return count;	
}

int Cdobs::put_object (istream &src, string name,
	string bucket_name, string &err_msg) {
	
	char ctime[MAX_TIME_LENGTH];
	// Get time as an "YYYY-MM-DD HH:MM:SS" format string
	int writ = get_current_time(ctime, MAX_TIME_LENGTH);
	int bucket_id = store->get_bucket_id(bucket_name.c_str());
	if (bucket_id < 0) {
		err_msg = ERR_INVALID_BUCKET_NAME + " " + bucket_name;
		return -1; // Also set bucket not exists error code.
	}
	int id = store->create_object(name.c_str(), 
		bucket_id, ctime, err_msg);
	if (id < 0) {
		return -1;
	}
	int size = store->put_object(src, id, err_msg);
	if (size == -1) {
		return -1; // Error status to object to large
	}
	store->update_object_size(id, size);
	return size;
}

int Cdobs::delete_object () {
	// To be implemented.
}

int Cdobs::list_objects () {
	// To be implemented.
}
