#include <iostream>
#include "sqlite3.h"
#include "Glob_Defs.h"
#include "RJM_SQLite_Resultset.h"

int main( int argc, char *argv[] )
{
	srand ( time(NULL) );
	printf("SQLite Demo program start\n");
	
	std::string l_filename = "datafile.sqlite";
	std::ostringstream l_query;
	sqlite3* l_sql_db = NULL;

	printf("Opening DB\n");
	int rc = sqlite3_open(l_filename.c_str(), &l_sql_db);
	if( rc ){
		sqlite3_close(l_sql_db);
		printf("Error couldn't open SQLite database %s",l_filename.c_str());
		return 1;
	};
	
	RJM_SQLite_Resultset *pRS = NULL;
	printf("Checking if table exists\n");
	pRS = SQL_Execute("SELECT name FROM sqlite_master WHERE type='table' and name='simple_table';", l_sql_db);	
	if (!pRS->Valid()) {
		printf("Invalid result set returned (%s)\n",pRS->GetLastError());
		SAFE_DELETE(pRS);
		sqlite3_close(l_sql_db);
		return 0;
	};
	rc = pRS->GetRowCount();
	SAFE_DELETE(pRS);
	if (0!=rc) {
		printf("Table exists\n");
	} else {
		printf("Table dosn't exist creating it\n");
		l_query.str("");
		l_query << "CREATE TABLE [simple_table] (";
		l_query << "[ID] INTEGER  NOT NULL PRIMARY KEY,";
		l_query << "[some_text] VARCHAR(255)  NULL,";
		l_query << "[some_number] INTEGER  NULL,";
		l_query << "[created] TIMESTAMP DEFAULT CURRENT_TIMESTAMP NULL";
		l_query << ")";
		pRS = SQL_Execute(l_query.str().c_str(), l_sql_db);	
		if (!pRS->Valid()) {
			printf("Invalid result set returned (%s)\n",pRS->GetLastError());
			SAFE_DELETE(pRS);
			sqlite3_close(l_sql_db);
			return 0;
		};
		
		printf("Checking if table exists\n");
		pRS = SQL_Execute("SELECT name FROM sqlite_master WHERE type='table' and name='simple_table';", l_sql_db);	
		if (!pRS->Valid()) {
			printf("Invalid result set returned (%s)\n",pRS->GetLastError());
			SAFE_DELETE(pRS);
			sqlite3_close(l_sql_db);
			return 0;
		};
		rc = pRS->GetRowCount();
		SAFE_DELETE(pRS);
		if (0==rc) {
			printf("Error table still dosn't exist despite the fact I created it\n",l_sql_db);
			sqlite3_close(l_sql_db);
			return 0;
		};
	}; //End If table dosn't exist
	SAFE_DELETE(pRS);
	
	printf("Add some data to the table\n");
	l_query.str("");
	l_query << "insert into simple_table (some_text, some_number)";
	l_query << " values ('Some text'," << GEN_ID << ")";
	pRS = SQL_Execute(l_query.str().c_str(), l_sql_db);	
	if (!pRS->Valid()) {
		printf("Invalid result set returned (%s)\n",pRS->GetLastError());
		SAFE_DELETE(pRS);
		sqlite3_close(l_sql_db);
		return 0;
	};
	SAFE_DELETE(pRS);

	printf("Listing out some data\n");
	l_query.str("");
	l_query << "select ID, some_text, some_number, created FROM simple_table";
	pRS = SQL_Execute(l_query.str().c_str(), l_sql_db);	
	if (!pRS->Valid()) {
		printf("Invalid result set returned (%s)\n",pRS->GetLastError());
		SAFE_DELETE(pRS);
		sqlite3_close(l_sql_db);
		return 0;
	};
	rc = pRS->GetRowCount();
	//Declare variables to hold data
	DB_DT_LONG r_id;
	DB_DT_VARCHAR r_some_text;
	DB_DT_LONG r_some_number;
	DB_DT_TIMESTAMP r_created;
	char buf[1024] = "";
	for (unsigned int c=0;c<rc;c++) {
		pRS->GetColValueINTEGER(c, 0, &r_id);
		pRS->GetColValueVARCHAR(c, 1, &r_some_text);
		pRS->GetColValueINTEGER(c, 2, &r_some_number);
		pRS->GetColValueTIMESTAMP(c, 3, &r_created);
		printf("ID: %d, Text: %s, Num: %d, Created: %s\n", r_id,r_some_text.c_str(),r_some_number,r_created.c_str());
	};
	SAFE_DELETE(pRS);
	
	printf("Closing DB\n");
	sqlite3_close(l_sql_db);
	
	printf("SQLite Demo program end\n");
	return 0;
};
