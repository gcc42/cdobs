#ifndef DBERROR_H
#define DBERROR_H

#include <string>

extern const std::string kErrDbNoinit;
extern const std::string kErrCdobsNoinit;

// Operations
extern const std::string kErrInitFailed;
extern const std::string kErrCreateBucketFailed;
extern const std::string kErrListBucketsFailed;
extern const std::string kErrDeleteBucketFailed;
extern const std::string kErrEmptyBucketFailed;
extern const std::string kErrPutObjectFailed;
extern const std::string kErrListObjectFailed;
extern const std::string kErrDeleteObjectFailed;

// setup db errors
extern const std::string kErrCantCreateFile;
extern const std::string kErrCantSetupTable;

// Cdobs errors
extern const std::string kErrEmptyingBucket;
extern const std::string kErrBucketAlreadyExists;
extern const std::string kErrObjectAlreadyExists;
extern const std::string kErrInvalidBucketName;
extern const std::string kErrInvalidObjectName;
extern const std::string kErrObjectTooLarge;
extern const std::string kErrInvalidSyntax;
extern const std::string kErrInvalidFile;

#endif