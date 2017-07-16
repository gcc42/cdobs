#include <iostream>
#include <cdobs.h>
#include <ctime>

using namespace std;

Cdobs::Cdobs (sqlite_store *store) : store(store) {
}

// Add any initializations here
int Cdobs::init () {
	if (store->init())
		return 1; // Call this here, or before passing to constructor?
	bucket_count = store->get_bucket_count();
	return 0;
}

int Cdobs::create_bucket (string name) {
	char ctime[MAX_TIME_LENGTH];
	// Get time as an "YYYY-MM-DD HH:MM:SS" format string
	int writ = get_current_time(ctime, MAX_TIME_LENGTH);
	int bucket_id = bucket_count++;
	int intial_obj_count = 0;

	char *bname = name.c_str();
	// Check if bucket name already exists
	if (store->get_bucket_id_from_name(bname) != -1)
		return 1;
	int err = store->insert_into_bucket(bucket_id, bname, ctime, intial_obj_count);
	return err;
}

// Also needs to delete all objects first.
int Cdobs::delete_bucket (string name) {
	int id = store->get_bucket_id_from_name(name.c_str());
	store->delete_all_objects_from_bucket(id);\
	if (id == -1)
		return 1;
	int err = store->delete_from_bucket(id);
	return err;
}

int Cdobs::list_buckets () {
	Bucket *buckets;
	int count = store->select_all_buckets(&buckets);
	
}