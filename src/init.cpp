#include <string>
#include "init.h"
#include "config.h"
#include "dberror.h"
#include "dbstore.h"
#include "cdobs.h"

using namespace std;
/* Inits DbStore object, passes it to cdobs,
 * and return the cdobs object */
int InitCdobs (Cdobs **cdobs, const string &db_file,
    string &err_msg) {
  DbStore *store = new DbStore();
  int ret_value = 0;
  // Init the DbStore object
  int rc = store->Init(db_file.c_str());
  if (rc) {
    err_msg = kErrDbNoinit;
    ret_value = 1;
  }
  else {
    // Init Cdobs
    *cdobs = new Cdobs(store);
    if (!(*cdobs)->good()) {
      err_msg = kErrCdobsNoinit;
      ret_value = 1;
    }   
  }
  if (ret_value) {
    *cdobs = NULL;
  }
  return ret_value;
}
