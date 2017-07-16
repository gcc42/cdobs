#include <utils.h>
#include <string>
#include <cstdio>
#include <ctime>

using namespace std;

string create_query (string format, string schema) {
	char buf[MAX_QUERY_SIZE];
	snprintf(buf, MAX_QUERY_SIZE, format, schema);
	return string(buf);
}

int get_current_time (char *buffer, int len) {
	time_t rawtime = time(nullptr);
	tm *current_time = gmtime(&rawtime);
	// SQLite expected date string format is "YYYY-MM-DD HH:MM:SS" (there are others too)
	int written = strftime(buffer, len, "%Y-%m-%d %H:%M:%S", currentTime);
	free(current_time); // Need to do this?
	return written;
}