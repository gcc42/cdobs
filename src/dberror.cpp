#include "dberror.h"
#include <string>

using namespace std;

// init_cdobs errors
const string ERR_DB_NOINIT = 
"Couldn't initialize database. \
Either the file doesn't exist or it has not \
been initialized. Run `cdobs init' to initialize database";

const string ERR_CDOBS_NOINIT = 
"Couldn't initialize cdobs. \
The database may be in an inconsistent state.";

// Command Error Messages
const string ERR_INIT_FAILED = "Operation `init' failed.";
const string ERR_CB_FAILED = "Operation `create_bucket' failed.";

// setup db errors
const string ERR_CANT_CREATE_FILE = 
"Couldn't create the database file, please check if \
the program has sufficient permissions";

const string ERR_CANT_SETUP_TABLE = 
"Sorry, there was a problem creating the sql tables. \
Has the database already been initialized? Try deleting \
the database file and run the command again.";

// Cdobs errors
const string ERR_BUCKET_ALREADY_EXIST = 
"Bucket already exists. \
Please choose a different name.";

const string ERR_INVALID_BUCKET_NAME =
"Invalid bucket name: ";

const string ERR_OBJECT_TOO_LARGE =
"Object size is too large. Max object size is: 5MB";

const string ERR_INVALID_SYNTAX = 
"Invalid Syntax: ";

const string ERR_INVALID_FILE = 
"Invalid File: ";