#include <string>
#include "init.h"
#include "config.h"
#include "dberror.h"
#include "dbstore.h"
#include "cdobs.h"

using namespace std;
/* Inits DbStore object, passes it to cdobs,
 * and return the cdobs object */
int InitCdobs (Cdobs **cdobs, string &err_msg) {
	DbStore *store = new DbStore();
	int ret_value = 0;
	// Init the DbStore object
	int rc = store->Init(SQLITE_DB_FILE);
	if (rc) {
		err_msg = ERR_DB_NOINIT;
		ret_value = 1;
	}
	else {
		// Init Cdobs
		*cdobs = new Cdobs(store);
		if (!(*cdobs)->good()) {
			err_msg = ERR_CDOBS_NOINIT;
			ret_value = 1;
		}		
	}
	if (ret_value) {
		*cdobs = NULL;
	}
	return ret_value;
}
