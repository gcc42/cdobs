#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
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

static const string HELP_OBJECT = 
"Usage: cdobs object \
[put -b bucket_name -f file_name object_name]\n";

void com_help () {
	cout << HELP_CDOBS;
}

void com_bucket_help () {
	cout << HELP_BUCKET;
}

void com_object_help () {
	cout << HELP_OBJECT;
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

int com_list_buckets (Cdobs *const cdobs) {
	vector<Bucket> buckets;
	string err_msg;
	int rc = cdobs->list_buckets(buckets, err_msg);
	if (rc) {
		cout << "ERROR: " << err_msg;
		return 1;
	}
	for (auto b = buckets.begin(); b != buckets.end(); ++b) {
		cout  << b->id << "   " << b->name << endl;
	}
	return 0;
}

int com_bucket (Cdobs *const cdobs, int argc, char **argv) {
	if (argc < 2) {
		return com_list_buckets(cdobs);
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

int com_put_object (Cdobs *const cdobs, int argc,
	char **argv) {
	string bucket_name, file_name, object_name;
	for (int i = 2; i < argc; ++i) {
		if (!std::strcmp(argv[i], "-b")) {
			bucket_name = argv[i + 1];
			i++;
		}
		else if (!std::strcmp(argv[i], "-f")) {
			file_name = argv[i + 1];
			i++;
		}
		else if (i == argc - 1) {
			object_name = argv[i];
		}
	}

	if (bucket_name == "" || object_name == ""
		|| file_name == "") {
		cout << ERR_INVALID_SYNTAX;
		com_object_help();
		return 1;
	}

	ifstream src(file_name.c_str(), ios::binary);
	if (!src.good()) {
		cout << "ERROR: " << ERR_INVALID_FILE
		<< file_name << endl;
		return 1;
	}

	string err_msg;
	int rc = cdobs->put_object(src, object_name,
		bucket_name, err_msg);
	if (rc) {
		cout << "ERROR: " << err_msg;
		return 1;
	}
	return 0;
}

int com_object (Cdobs *const cdobs, int argc, char **argv) {
	if (argc < 2) {
		com_object_help();
		return 0;
	}
	string err_msg, arg1(argv[1]);
	if (arg1 == "put") {
		return com_put_object(cdobs, argc, argv);
	}
	else {
		com_object_help();
		return 1;
	}
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

	int exit_code = 0;	
	string arg1(argv[1]);
	if (arg1 == "init") {
		exit_code = com_init();
	}
	else  {
		Cdobs *cdobs;
		int retValue = 0;
		string err_msg;
		int rc_init = init_cdobs(&cdobs, err_msg);
		if (rc_init) {
			cout << "ERROR: " << err_msg;
			exit_code = 1;
		}
		else {
			if (arg1 == "bucket") {
				// send the rest of commands
				exit_code = com_bucket(cdobs, argc - 1, argv + 1);
			}
			else if (arg1 == "object") {
				exit_code = com_object(cdobs, argc - 1, argv + 1);
			}
			else {
				com_help();
				exit_code = 1;
			} 			
		}
	}

	exit(exit_code);
}
