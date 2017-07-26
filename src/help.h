#include <string>

const std::string kHelpCdobs =
"Usage: cdobs [OPTION] bucket OPERATION BUCKET_NAME\n\
    or: cdobs [OPTION] object OPERATION [OBJ_OPTIONS]... OBJECT\n\
Perform the specific object or bucket storage operation on\n\
an sqlite database\n\
\n\
OPTION can be one of the following:\n\
      --db DB_NAME             Name of the sqlite database file\n\
      --help     display this help and exit\n\
      --version  output version information and exit\n\
\n\
1. cdobs [OPTION] bucket OPERATION BUCKET_NAME\n\
    \n\
Where BUCKET_NAME is the name of the bucket. OPERATION can be:\n\
        create      Create a bucket\n\
        delete      Delete a bucket. If the bucket is non empty,\n\
                    then this operation also deletes all the objects\n\
                    in the bucket\n\
        empty       Deletes all objects in the bucket\n\
\n\
Running 'cdobs bucket' will list all buckets in storage.\n\
\n\
2. cdobs [OPTION] object OPERATION [OBJ_OPTION]... OBJECT\n\
Again, OBJECT_NAME is the name of the object to perform the operation on.\n\
\n\
OPERATION can be:\n\
        put         Put an object in the specified bucket. Buckets are\n\
                    specified using the -b flag (see below)\n\
        list        List all objects in bucket\n\
        delete      Delete object from Bucket\n\
\n\
OBJ_OPTION can be the following:\n\
        -b BUCKET_NAME      Specify the bucket to which OBJECT belongs. This\n\
                            is mandatory for all object operations\n\
        -f FILE             Name of the file from which to get object data. This\n\
                            needs to be along with the fully qualified path.\n\
"
;