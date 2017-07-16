 /* The cdobs class : The Ceph Database object storage. 
  * Used in place of librados to store RGW(S3) objects in an 
  * sql database */ 
class Cdobs {
private:
	sqlite_store *store;
	int bucket_count;
	int state;
public:
	Cdobs();

	// Create a bucket sstruct?
	/* Bucket operations */
	int create_bucket();
	int delete_bucket_by_name();
	int list_buckets();

	/* Object operations */
	int insert_object();
	int delete_object();
	int list_objects();
}

struct Bucket {
	int id;
	string name;
	struct tm *created;
	int object_count;
};