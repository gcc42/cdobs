#include <iostream>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

#define SQLITE_DB_FILE "cdobs.sqlite"
#define DB_PATH "./"  // Create database on the current path

#define MAX_TIME_LENGTH 100

const std::string SQL_CREATE = "CREATE TABLE %s;";

const std::string BUCKET_SCHEMA = "Bucket ( \
                                      BucketID INTEGER PRIMARY KEY, \
                                      BucketName VARCHAR UNIQUE, \
                                      Created DATE, \
                                      ObjectCount INTEGER \
                                    )";
                                        // Bucket ACL: ??,
                                        // BuckerOwner: Interger REFERENCES User.Uid,
                                        // Location: VARCHAR,

const std::string OBJECT_DIR_SCHEMA = "ObjectDirectory ( \
                                              ObjectID VARCHAR PRIMARY KEY, \
                                              ObjectName TEXT, \
                                              BucketId INTEGER REFERENCES Bucket, \
                                              Created DATE, \
                                              Size INTEGER \
                                            )";

const std::string OBJECT_STORE_SCHEMA = "ObjectStore ( \
                                          ObjectID VARCHAR REFERENCES ObjectDirectory, \
                                          Data BLOB, \
                                          PRIMARY KEY(ObjectID) \
                                        )";                                      
#endif