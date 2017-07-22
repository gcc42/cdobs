#ifndef UTILS_H
#define UTILS_H

#include <string>

#define TIME_FORMAT "%Y-%m-%d %H:%M:%S"

std::string CreateQuery(std::string format, std::string schema);
int GetCurrentTime (char *buffer, int len);
struct tm *ParseTimeString (char *time);

#endif