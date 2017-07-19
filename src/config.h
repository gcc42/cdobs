#include <iostream>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

#define SQLITE_DB_FILE "cdobs.sqlite"
#define DB_PATH "./"  // Create database on the current path

#define MAX_TIME_LENGTH 50

#define MAX_QUERY_SIZE 1000

const std::string sql_create = "CREATE TABLE %s;";

const std::string bucket_schema = "Bucket (
                                      BucketID: INTEGER PRIMARY KEY,
                                      BucketName: VARCHAR UNIQUE,
                                      Created: DATE,
                                      ObjectCount: INTEGER
                                    )";
                                        // Bucket ACL: ??,
                                        // BuckerOwner: Interger REFERENCES User.Uid,
                                        // Location: VARCHAR,

const std::string object_directory_schema = "ObjectDirectory (
                                              ObjectID: VARCHAR PRIMARY KEY,
                                              ObjectName: TEXT,
                                              BucketId: INTEGER REFERENCES Bucket,
                                              Created: DATE,
                                              Size: INTEGER
                                            )";

const std::string object_store_schema = "ObjectStore (
                                          ObjectID: VARCHAR REFERENCES ObjectDirectory, 
                                          Data: BLOB,
                                          PRIMARY KEY(ObjectID)
                                        )";                                      
#endif