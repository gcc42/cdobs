#include <iostream>
#include <string>
#include "config.h"
#include "init.h"
#include "create.h"
#include "dbstore.h"
#include "cdobs.h"
#include "dberror.h"

using namespace std;

int command_help () {
	string help_msg = "Usage: ";
	cout << help_msg;
	return 0;
}

int command_init () {
	string err_msg;
	int rc = setup_database(err_msg);
	if (rc) {
		cerr << "ERROR: " << ERR_INIT_FAILED
		<< err_msg;
		return 1;
	}
	return 0;
}


int command_create_bucket (int argc, char **argv) {
	Cdobs *cdobs;
	int retValue = 0;
	string bucket_name(argv[2]), err_msg;
	int rc_init = init_cdobs(&cdobs, err_msg);
	if (rc_init) {
		retValue = 1;
	}
	else {
		int rc_cb = cdobs->create_bucket(bucket_name, err_msg);
		if (rc_cb) {
			retValue = 1;		
		}		
	}
	if (retValue) {
		cerr << "ERROR: " << ERR_CB_FAILED
		<< err_msg;
	}
	return retValue;
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
	int rc, exit_code = 0;
	string arg1(argv[1]);
	if (arg1 == "init") {
		rc = command_init();
		exit_code = rc;
	}
	else if (arg1 == "create_bucket") {
		rc = command_create_bucket(argc, argv);
		exit_code = rc;
	}
	else {
		command_help();
		exit_code = 1;
	}
	exit(exit_code);
}
