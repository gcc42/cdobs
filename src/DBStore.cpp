// RJM_SQLite_Resultset.cpp: implementation of the RJM_SQLite_Resultset class.
//
//////////////////////////////////////////////////////////////////////

#include "RJM_SQLite_Resultset.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RJM_SQLite_Resultset::RJM_SQLite_Resultset()
{
	bValid = true; //assume valid until set error is called by sql_execute function
	RowCount = 0;
}

RJM_SQLite_Resultset::~RJM_SQLite_Resultset()
{
	ClearRes();
}

int RJM_SQLite_Resultset::sql_callback(int argc, char **argv, char **azColName)
{
	bValid = true;
	int c;
	std::string *pStr;
	RJM_SQLite_ResultsetRow* pRS = new RJM_SQLite_ResultsetRow();
	if (svColNames.size()==0) {
		svColNames.reserve(argc);
		//set up col names
		for (c=0;c<argc;c++) {
			pStr = new std::string(azColName[c]);
			svColNames.push_back(pStr);
		};
	};
	//Add the columns to this row
	for (c=0;c<argc;c++) {
		if (argv[c]==0) {
			pRS->AddValue("");
		} else {
			pRS->AddValue(argv[c]);
		};
	};

	m_rvRows.push_back(pRS);
	RowCount++;
	return 0;
}

void RJM_SQLite_Resultset::ClearRes()
{
	for (StringVectorItor itor = svColNames.begin();itor!=svColNames.end();++itor) {
		delete *itor;
	};
	for (RJM_RSRowVectorrItor itor2 = m_rvRows.begin();itor2!=m_rvRows.end();++itor2) {
		delete *itor2;
	};
}

void RJM_SQLite_Resultset::GetColName(int c, std::string *ret)
{
	ASSERT(c>-1);
	ASSERT(c<svColNames.size());

	ret->assign(*svColNames.at(c));
}

void RJM_SQLite_Resultset::GetColValueVARCHAR(int Row, int Col, std::string *val)
{
	m_rvRows[Row]->GetValue(Col, val);
}

void RJM_SQLite_Resultset::GetColValueTIMESTAMP(int Row, int Col, DB_DT_TIMESTAMP_PASS val)
{
	std::string tmp;
	GetColValueVARCHAR(Row,Col,&tmp);
	if (tmp=="NULL") {
		*val = DB_DT_TIMESTAMP_DEF;
		return;
	};
	DB_DT_TIMESTAMP *pTS = GetTIMESTAMPti(tmp.c_str());
	*val = *pTS;
	SAFE_DELETE(pTS); 
}

void RJM_SQLite_Resultset::GetColValueINTEGER(int Row, int Col, DB_DT_LONG *val)
{
	std::string tmp;
	GetColValueVARCHAR(Row,Col,&tmp);
	if (tmp=="") {
		*val=0; //Read Nulls as 0
	} else {
		sscanf(tmp.c_str(),"%d",val);
	};
}

void RJM_SQLite_Resultset::GetColValueBLOB(int Row, int Col, DB_DT_VARCHAR *val)
{
	m_rvRows[Row]->GetValue(Col, val);
}

bool RJM_SQLite_Resultset::ValidUserDate(const char *DateStr, DB_DT_TIMESTAMP_PASS pT)
{
	DB_DT_TIMESTAMP a(DateStr,"DD-MON-YYYY");
	if (a.valid()) {
		*pT = a;
		return true;
	};
	return false;
}

bool RJM_SQLite_Resultset::ValidUserDateDecodeDate(const char *DateStr, tm *ts)
{
	if (DateStr[2]!='-') return false;
	if (DateStr[6]!='-') return false;
	int d=0;
	int m=0;
	int y=0;
	char d_buf[3];
	char m_buf[4];
	char y_buf[5];
	d_buf[0] = DateStr[0];
	d_buf[1] = DateStr[1];
	d_buf[2] = '\0';
	m_buf[0] = toupper(DateStr[3]);
	m_buf[1] = toupper(DateStr[4]);
	m_buf[2] = toupper(DateStr[5]);
	m_buf[3] = '\0';
	y_buf[0] = DateStr[7];
	y_buf[1] = DateStr[8];
	y_buf[2] = DateStr[9];
	y_buf[3] = DateStr[10];
	y_buf[4] = '\0';
	sscanf(d_buf,"%d",&d);
	sscanf(y_buf,"%d",&y);

	switch (m_buf[0]) {
	case 'J':
		//jan jun jul
		if (m_buf[1]=='U') {
			if (m_buf[2]=='N') m=6; //JUN
			if (m_buf[2]=='L') m=7; //JUL
		} else {
			if (m_buf[1]=='A') {
				if (m_buf[2]=='N') m=1; //JAN
			};
		};
		break;
	case 'F':
		if (m_buf[1]=='E') if (m_buf[2]=='B') m=2; //feb
		break;
	case 'M':
		if (m_buf[1]=='A') {
			if (m_buf[2]=='R') m=3; //MAR
			if (m_buf[2]=='Y') m=5; //MAR
		};
		break;
	case 'A':
		if (m_buf[1]=='P') {
			if (m_buf[2]=='R') m=4; //APR
		} else {
			if (m_buf[1]=='U') if (m_buf[2]=='G') m=8; //AUG
		};
		break;
	case 'S':
		if (m_buf[1]=='E') if (m_buf[2]=='P') m=9; //sep
		break;
	case 'O':
		if (m_buf[1]=='C') if (m_buf[2]=='T') m=10; //oct
		break;
	case 'N':
		if (m_buf[1]=='O') if (m_buf[2]=='V') m=11; //nov
		break;
	case 'D':
		if (m_buf[1]=='E') if (m_buf[2]=='C') m=12; //dec
		break;
	default:
		m=0;
	};

	if (d<1) return false;
	if (d>31) return false;
	if (m<1) return false;
	if (m>12) return false;
	if (y<1900) return false;
	if (y>2500) return false;
	ts->tm_mday = d;
	ts->tm_mon = m-1;
	ts->tm_year = y -1900;

	return true;
}
