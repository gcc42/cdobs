#ifndef UTILS_H
#define UTILS_H

#include <string>

#define TIME_FORMAT "%Y-%m-%d %H:%M:%S"

std::string create_query(std::string format, std::string schema);
int get_current_time (char *buffer, int len);
struct tm *parse_time_string (char *time);

#endif