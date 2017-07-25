#include "config.h"
#include "dberror.h"
#include <string>

using namespace std;

// init_cdobs errors
const string kErrDbNoinit = 
"Couldn't read database. \
Either the database file doesn't exist or it has not \
been initialized. Run `cdobs init' to create and initialize database";

const string kErrCdobsNoinit = 
"Couldn't initialize cdobs. \
The database may be in an inconsistent state.";

// Command Error Messages
const string kErrInitFailed = "Operation `init' failed: ";
const string kErrCreateBucketFailed = "Operation `create_bucket' failed: ";
const string kErrListBucketsFailed = "Operation `list buckets' failed: ";
const string kErrDeleteBucketFailed = "Operation `delete buckets' failed: ";
const string kErrPutObjectFailed = "Operation `put object' failed: ";
const string kErrListObjectFailed = "Operation `list object' failed: ";
const string kErrDeleteObjectFailed = "Operation `delete object' failed: ";

// setup db errors
const string kErrCantCreateFile = 
"Couldn't create the database file, please check if \
the program has sufficient permissions";

const string kErrCantSetupTable = 
"Sorry, there was a problem creating the sql tables. \
Has the database already been initialized? Try deleting \
the database file and run the command again.";

// Cdobs errors
const string kErrBucketAlreadyExists = 
"Bucket already exists. \
Please choose a different name.";

const string kErrObjectAlreadyExists = 
"An object already exists in this bucket with name: ";

const string kErrInvalidBucketName =
"Invalid bucket name: ";

const string kErrInvalidObjectName =
"No such object in bucket: ";

const string kErrObjectTooLarge =
"Object size is too large. Max object size is: 5 Mb";

const string kErrInvalidSyntax = 
"Invalid Syntax: ";

const string kErrInvalidFile = 
"Invalid File: ";

const string kErrEmptyingBucket =
"Error deleting objects from bucket: ";