#ifndef CREATE_H
#define CREATE_H

#define FAILURE -1
#define SUCCESS 0

#include <string>

/* Takes config from file config.h, for now */
int setup_database (std::string &err_msg);
#endif