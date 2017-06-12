
#include "stdafx.h"

#include "mdspi.h"
#pragma warning(disable : 4996)

extern int requestId;  
extern HANDLE g_hEvent;

void CtpMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
						  int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

void CtpMdSpi::OnFrontDisconnected(int nReason)
{
	cerr<<" ��Ӧ | �����ж�..." 
		<< " reason=" << nReason << endl;
}

void CtpMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<" ��Ӧ | ������ʱ����..." 
		<< " TimerLapse = " << nTimeLapse << endl;
}

void CtpMdSpi::OnFrontConnected()
{
	cerr<<" ���ӽ���ǰ��...�ɹ�"<<endl;
	SetEvent(g_hEvent);
}

void CtpMdSpi::ReqUserLogin(TThostFtdcBrokerIDType	appId,
							TThostFtdcUserIDType	userId,	TThostFtdcPasswordType	passwd)
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, appId);
	strcpy(req.UserID, userId);
	strcpy(req.Password, passwd);
	int ret = pUserApi->ReqUserLogin(&req, ++requestId);
	cerr<<" ���� | ���͵�¼..."<<((ret == 0) ? "�ɹ�" :"ʧ��") << endl;	
	SetEvent(g_hEvent);
}

void CtpMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin)
	{
		cerr<<" ��Ӧ | ��¼�ɹ�...��ǰ������:"
			<<pRspUserLogin->TradingDay<<endl;
	}
	if(bIsLast) SetEvent(g_hEvent);
}

void CtpMdSpi::SubscribeMarketData(char* instIdList)
{
	vector<char*> list;
	char *token = strtok(instIdList, ",");
	while( token != NULL ){
		list.push_back(token); 
		token = strtok(NULL, ",");
	}
	unsigned int len = list.size();
	char** pInstId = new char* [len];  
	for(unsigned int i=0; i<len;i++)  pInstId[i]=list[i]; 
	int ret=pUserApi->SubscribeMarketData(pInstId, len);
	cerr<<" ���� | �������鶩��... "<<((ret == 0) ? "�ɹ�" : "ʧ��")<< endl;
	SetEvent(g_hEvent);
}

void CtpMdSpi::OnRspSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr<<" ��Ӧ |  ���鶩��...�ɹ�"<<endl;
	if(bIsLast)  SetEvent(g_hEvent);
}

void CtpMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr<<" ��Ӧ |  ����ȡ������...�ɹ�"<<endl;
	if(bIsLast)  SetEvent(g_hEvent);
}

void CtpMdSpi::string_replace(string&s1,const string&s2,const string&s3)
{
	string::size_type pos=0;
	string::size_type a=s2.size();
	string::size_type b=s3.size();
	while((pos=s1.find(s2,pos))!=string::npos)
	{
		s1.replace(pos,a,s3);
		pos+=b;
	}
}

string CtpMdSpi::getDBNameFromInstrumentID(TThostFtdcInstrumentIDType InstrumentID)
{
	char buf[16] = {0};
	// http://blog.chinaunix.net/uid-26284412-id-3189214.html
	//  sscanf�ĸ߼��÷�
	sscanf(InstrumentID, "%[^0-9]", buf);
	string strDbName = string(buf)+string("_DB");
	return strDbName;
}

string CtpMdSpi::getDbSQLFromInstrumentID(
	TThostFtdcInstrumentIDType InstrumentID)
{
	string strDbName = getDBNameFromInstrumentID(InstrumentID);
	string strSQL = "create table if not exists TableNameXXX(Contracts varchar(20), date datetime, \
					Open numeric(15,2), High numeric(15,2),Low numeric(15,2),Close numeric(15,2), Volume numeric(25,2),OpenInt numeric(25,2), Turnover numeric(25,2), \
					PreSettlement numeric(15,2), PreClose numeric(15,2), PreOpenInt numeric(15,2));";
	// create index IF_DB_INDEX on IF_DB(Contracts, date);
	
	string_replace(strSQL, "TableNameXXX", strDbName);
	return strSQL;
}

void CtpMdSpi::updateSQLiteData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	string sSQL = "insert into IF_DB values(2, null, 200, 2.22);";
	char buf[1024] = {0};
	sprintf(buf,"insert into %s values(2, null, 200, 2.22);", getDBNameFromInstrumentID(pDepthMarketData->InstrumentID).c_str());  
}

void CtpMdSpi::OnRtnDepthMarketData(
	CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cerr<<"MdSpi ���� | ��Լ:"<<pDepthMarketData->InstrumentID
		<<" �ּ�:"<<pDepthMarketData->LastPrice
		<<" ��߼�:" << pDepthMarketData->HighestPrice
		<<" ��ͼ�:" << pDepthMarketData->LowestPrice
		<<" ��һ��:" << pDepthMarketData->AskPrice1 
		<<" ��һ��:" << pDepthMarketData->AskVolume1 
		<<" ��һ��:" << pDepthMarketData->BidPrice1
		<<" ��һ��:" << pDepthMarketData->BidVolume1
		<<" �ֲ���:"<< pDepthMarketData->OpenInterest <<endl;

	// �����ݿ���Ҷ�Ӧ�ı����û�ж�Ӧ����򴴽����
	string strSql = getDbSQLFromInstrumentID(pDepthMarketData->InstrumentID);
	m_db.execDML(strSql.c_str());
	m_db.execDML("insert into IF_DB values('IF1706', '2016-06-12', 200, 300, 400, 500, 1000, 2000, 3000, 4000, 5000, 6000);");
	
	// �ӱ���ѯ���յ�IF K�ߣ�û���򴴽�
	// ����k�ߵĿ��ߵ���
	// 
	//CppSQLite3DB db;

	//cout << "SQLite Header Version: " << CppSQLite3DB::SQLiteHeaderVersion() << endl;
	//cout << "SQLite Library Version: " << CppSQLite3DB::SQLiteLibraryVersion() << endl;
	//cout << "SQLite Library Version Number: " << CppSQLite3DB::SQLiteLibraryVersionNumber() << endl;

	////remove(gszFile);
	//db.open(gszFile);
	//db.execDML("create table if not exists if_db(Contracts varchar(20), date datetime, \
	//		   Open numeric(15,2), High numeric(15,2),Low numeric(15,2),Close numeric(15,2), Volume numeric(25,2),OpenInt numeric(25,2), Turnover numeric(25,2), \
	//		   PreSettlement numeric(15,2), PreClose numeric(15,2), PreOpenInt numeric(15,2));");
	// 
	// 
	// 
	// 
	// 
	// 
	// 
}

bool CtpMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{	
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (ret){
		cerr<<" ��Ӧ | "<<pRspInfo->ErrorMsg<<endl;
	}
	return ret;
}