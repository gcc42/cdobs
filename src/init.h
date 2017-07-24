#ifndef INIT_H
#define INIT_H

#include <string>
#include "cdobs.h"

int InitCdobs (Cdobs **cdobs, const std::string &db_file,
	std::string &err_msg);
#endif