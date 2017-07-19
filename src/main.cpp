#include <iostream>
#include <string>
#include "config.h"
#include "init.h"
#include "create.h"
#include "dbstore.h"
#include "cdobs.h"
#include "dberror.h"

using namespace std;


int command_init (string &err_msg) {
	int rc = setup_database();
	if (rc) {
		err_msg = ERR_INIT_FAILED;
		return l;
	}
	return 0;
}


int command_create_bucket (int argc, char **argv,
	string &err_msg) {
	Cdobs *cdobs;
	string bucket_name(argv[2]);
	int rc_init = init_cdobs(cdobs, err_msg);
	if (rc_init) {
		err_msg = "In operation create_bucket: " + err_msg;
		return 1;
	}
	int rc_cb = cdobs->create_bucket(bucket_name);
	if (rc_cb) {
		err_msg = "In operation create_bucket: Create bucket failed";
		return 1;		
	}
	return 0;
}

// TODO: 
// 1. Add a help message.
// 2. Case insentivity, etc. 
// 2. Use a proper parser to parse
// arguments. 
int main(int argc, char **argv) {
	if (argc < 2) {
		exit(0);
	}
	int rc;
	string arg1(argv[1]), err_msg;
	if (arg1 == "init") {
		rc = command_init(err_msg);
		if (rc) {
			cout << "ERROR: " << err_msg << endl;
			exit(1);
		}
		exit(0);
	}
	else if (arg1 == "create_bucket") {
		rc = command_create_bucket(argc, argv, err_msg);
		if (rc) {
			cout << "ERROR: " << err_msg;
			exit(1); 
		}
		exit(0);
	}
	else {
		// Show help message
		exit(1);
	}
}
