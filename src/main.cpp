#include <iostream>
#include <string>
#include "config.h"
#include "init.h"
#include "create.h"
#include "dbstore.h"
#include "cdobs.h"
#include "dberror.h"

using namespace std;

static const string HELP_BUCKET = 
"Usage: cdobs bucket \
[help | create name | \
list name | delete name]\n\
where name is the bucket_name\n";

static const string HELP_CDOBS = 
"Usage: cdobs \
[help | bucket | object]\n\
Hit one of the commands to know more \
about them \n";


void com_help () {
	cout << HELP_CDOBS;
}

void com_bucket_help () {
	cout << HELP_BUCKET;
}

int com_init () {
	string err_msg;
	int rc = setup_database(err_msg);
	if (rc) {
		cerr << "ERROR: " << ERR_INIT_FAILED
		<< err_msg;
		return 1;
	}
	return 0;
}


int com_create_bucket (Cdobs *const cdobs, int argc,
	char **argv) {
	int retValue = 0;
	string bucket_name(argv[2]), err_msg;
	int rc_cb = cdobs->create_bucket(bucket_name, err_msg);
	if (rc_cb) {
		retValue = 1;		
		cerr << "ERROR: " << ERR_CB_FAILED
		<< err_msg;
	}		

	return retValue;
}

int com_bucket (int argc, char **argv) {
	if (argc < 2) {
		com_bucket_help();
		return 0;
	}

	Cdobs *cdobs;
	int retValue = 0;
	string err_msg;
	int rc_init = init_cdobs(&cdobs, err_msg);
	if (rc_init) {
		cout << "ERROR: " << err_msg;
		return 1;
	}
	string arg1(argv[1]); int rc_op;
	if (arg1 == "create") {
		rc_op = com_create_bucket(cdobs, argc, argv);
	}
	else {
		com_bucket_help();
		return 1;
	}
}

int com_object (int argc, char **argv) {
	// TBI
}


// TODO: 
// 1. Add a help message.
// 2. Case insentivity, etc. 
// 2. Use a proper parser to parse
// arguments. 
int main(int argc, char **argv) {
	dout << "Starting program cdobs" << endl; 
	if (argc < 2) {
		com_help();
		exit(0);
	}
	int rc, exit_code = 0;
	string arg1(argv[1]);
	if (arg1 == "init") {
		rc = com_init();
		exit_code = rc;
	}
	else if (arg1 == "bucket") {
		// Send the rest of commands
		rc = com_bucket(argc - 1, argv + 1);
		exit_code = rc;
	}
	else if (arg1 == "object") {
		rc = com_object(argc, argv);
		exit_code = rc;
	}
	else {
		com_help();
		exit_code = 1;
	}
	exit(exit_code);
}
