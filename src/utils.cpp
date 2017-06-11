#include <utils.h>
#include <string>
#include <cstdio>

using namespace std;

string create_query(string format, string schema) {
	char buf[MAX_QUERY_SIZE];
	snprintf(buf, MAX_QUERY_SIZE, format, schema);
	return string(buf);
}