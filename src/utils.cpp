#include "utils.h"
#include <string>
#include <cstdio>
#include <ctime>
#include "dbstore.h"

using namespace std;

string create_query (string format, string schema) {
	char buf[MAX_QUERY_SIZE];
	snprintf(buf, MAX_QUERY_SIZE, format.c_str(), schema.c_str());
	return string(buf);
}

int GetCurrentTime (char *buffer, int len) {
	time_t rawtime = time(nullptr);
	tm *current_time = gmtime(&rawtime);
	// SQLite expected date string format is "YYYY-MM-DD HH:MM:SS" (there are others too)
	int written = strftime(buffer, len, TIME_FORMAT, current_time);
	// free(current_time); // Why is this segaulting?
	return written;
}

tm *parse_time_string (char *time) {
	struct tm *stime = new struct tm;
	char *err = strptime(time, TIME_FORMAT, stime);
	if (err == NULL) {
		return NULL;
	}
	return stime;
}
