// RJM_SQLite_Resultset.h: interface for the RJM_SQLite_Resultset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RJM_SQLITE_RESULTSET_H__24EF43D6_30FC_4E10_80FC_6BAF3801EDBA__INCLUDED_)
#define AFX_RJM_SQLITE_RESULTSET_H__24EF43D6_30FC_4E10_80FC_6BAF3801EDBA__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable : 4786)

//This class holds a SQL_Lite Query result

#include "sqlite3.h"
#include <string>
#include <vector>
#include <time.h>
#include "RJMFTime.h"
#include <sstream>
#include "Glob_Defs.h"

typedef std::vector<std::string*> StringVector;
typedef StringVector::iterator StringVectorItor;
	
class RJM_SQLite_ResultsetRow
{
public:
	int GetSize() {return svRowValues.size();};
	void GetValue(int c, std::string *val) {
		val->assign(*svRowValues.at(c));
	};
	void AddValue(std::string val) {
		std::string *pStr;
		pStr = new std::string;
		*pStr = val;
		svRowValues.push_back(pStr);
	};
	virtual ~RJM_SQLite_ResultsetRow() {
		for (StringVectorItor itor = svRowValues.begin();itor!=svRowValues.end();++itor) {
			delete *itor;
		};
	};
private:
	StringVector svRowValues;
	
};

typedef std::vector<RJM_SQLite_ResultsetRow*> RJM_RSRowVector;
typedef RJM_RSRowVector::iterator RJM_RSRowVectorrItor;

class RJM_SQLite_Resultset  
{
public:
	static bool ValidUserDate(const char* DateStr, DB_DT_TIMESTAMP_PASS pT);
	void GetColValueBLOB(int Row, int Col, DB_DT_VARCHAR *val);
	void GetColValueINTEGER(int Row, int Col, DB_DT_LONG *val);
	void GetColValueTIMESTAMP(int Row, int Col, DB_DT_TIMESTAMP *val);
	void GetColValueVARCHAR(int Row, int Col, std::string *val);

	void GetColName(int c, std::string *ret);
	int sql_callback(int argc, char **argv, char **azColName);
	RJM_SQLite_Resultset();
	virtual ~RJM_SQLite_Resultset();

	bool Valid() {return bValid;};
	void SetError(char* err) {bValid = false; Last_Error = err;};
	const char* GetLastError() {return Last_Error.c_str();};
	int GetRowCount() {return RowCount;};
	int GetNumCols() {return svColNames.size();};

	static DB_DT_TIMESTAMP_RET GetTIMESTAMPti (const char* SQLStr) {
		return new RJMF_TimeStamp(SQLStr, "YYYY-MM-DD HH:MM:SS");
	};
	static std::string GetTIMESTAMPstr(DB_DT_TIMESTAMP* ct) {
		if (*ct==DB_DT_TIMESTAMP_DEF) return "NULL";

		//Given a time return a string with it in a timestamp format
		//2006-08-13 10:41:16
		char buf[255];
		ct->strftime(buf,"%Y-%m-%d %H:%M:%S");

		std::ostringstream strstrm;
		strstrm.str("");
		strstrm << buf;

		return strstrm.str();
		//return "2002-01-01 02:34:56";
	};
	static std::string getSQLOKString(const char* buf) {
		std::string r = "";
		int c=0;
		while (!(buf[c]=='\0')) {
			r = r + buf[c];
			if (buf[c]=='\'') r=r+'\'';
			c++;
		};
		return r;
	};
	static std::string getUserDate(DB_DT_TIMESTAMP_PASS ct) {
		std::ostringstream strstrm;
		strstrm.str("");
		char buf[20];
		ct->strftime(buf, "%d-%b-%Y");
		strstrm << buf;
		return strstrm.str();
	};
	static std::string getUserDateTime(DB_DT_TIMESTAMP_PASS ct) {
		if (*ct==DB_DT_TIMESTAMP_DEF) return "NULL";

		std::ostringstream strstrm;
		strstrm.str("");

		char buf[40];
		ct->strftime(buf, "%d-%b-%Y %H:%M:%S");
		strstrm << buf;
		return strstrm.str();

	};

private:
	static bool ValidUserDateDecodeDate(const char *DateStr, tm *ts);
	void ClearRes();

	int RowCount;
	bool bValid;
	std::string Last_Error;

	StringVector svColNames;
	RJM_RSRowVector m_rvRows;
};


static int sql_callback(void *NotUsed, int argc, char **argv, char **azColName){
	RJM_SQLite_Resultset *pRS = (RJM_SQLite_Resultset*) NotUsed;
	return pRS->sql_callback(argc,argv,azColName);
}

static RJM_SQLite_Resultset* SQL_Execute(const char* cmd, sqlite3* db) {
	RJM_SQLite_Resultset* pRS;
	pRS = new RJM_SQLite_Resultset();
	
	int rc;
	char *zErrMsg = 0;
	rc = sqlite3_exec(db, cmd, sql_callback, pRS, &zErrMsg);
	if( rc!=SQLITE_OK ){
		pRS->SetError(zErrMsg);
		sqlite3_free(zErrMsg);
	}

	return pRS;
};

#endif // !defined(AFX_RJM_SQLITE_RESULTSET_H__24EF43D6_30FC_4E10_80FC_6BAF3801EDBA__INCLUDED_)
