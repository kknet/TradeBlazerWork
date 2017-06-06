// getCtpData2SQLite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "sfit/include/ThostFtdcMdApi.h"
#include "sfit/include/ThostFtdcTraderApi.h"

#include "mdspi.h"
#include "traderspi.h"

//
HANDLE g_hEvent;

//
string strNeedQuote;

// ������
int requestId=0;

// ǰ�õ�ַ
//char mdFront[]   ="tcp://180.168.212.229:41213";
//char tradeFront[]="tcp://180.168.212.229:41205";
//TThostFtdcBrokerIDType		appId	= "8080";
//TThostFtdcUserIDType		userId	= "";
//TThostFtdcPasswordType	    passwd	= "";
char mdFront[]   ="tcp://61.144.241.113:41213";
char tradeFront[]="tcp://61.144.241.113:41205";
TThostFtdcBrokerIDType		appId	= "8090";
TThostFtdcUserIDType		userId	= "";
TThostFtdcPasswordType	    passwd	= "";

int _tmain(int argc, _TCHAR* argv[])
{
	g_hEvent=CreateEvent(NULL, true, false, NULL); 

	//��ʼ��UserApi
	CThostFtdcMdApi* pMdUserApi=CThostFtdcMdApi::CreateFtdcMdApi();
	CtpMdSpi* pMdUserSpi=new CtpMdSpi(pMdUserApi); //�����ص����������MdSpi
	pMdUserApi->RegisterSpi(pMdUserSpi);			 // �ص�����ע��ӿ���
	pMdUserApi->RegisterFront(mdFront);		     // ע������ǰ�õ�ַ
	pMdUserApi->Init();      //�ӿ��߳�����, ��ʼ����	
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	//��ʼ��UserApi
	CThostFtdcTraderApi* pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
	CtpTraderSpi* pTraderUserSpi = new CtpTraderSpi(pTraderUserApi);
	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// ע���¼���
	pTraderUserApi->SubscribePublicTopic(THOST_TERT_RESTART);					// ע�ṫ����
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);			  // ע��˽����
	pTraderUserApi->RegisterFront(tradeFront);							// ע�ύ��ǰ�õ�ַ
	pTraderUserApi->Init();
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	// ��¼����
	pMdUserSpi->ReqUserLogin(appId,userId,passwd);
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	// ��¼����
	pTraderUserSpi->ReqUserLogin(appId,userId,passwd);
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	// ȷ�Ͻ��㵥
	pTraderUserSpi->ReqSettlementInfoConfirm();
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	// ��ѯ��Լ
	TThostFtdcInstrumentIDType instId = "";
	pTraderUserSpi->ReqQryInstrument(instId);
	WaitForSingleObject(g_hEvent,INFINITE);
	ResetEvent(g_hEvent);

	//
	//pMdUserSpi->SubscribeMarketData((char*)strNeedQuote.c_str());

	//pMdUserApi->Join();      //�ȴ��ӿ��߳��˳�
	//pTraderUserApi->Join();  

	printf("Input q to quit!");
	char ch;
	ch = getchar();
	if(ch == 'q') 
		exit(0);

	//system("pause");
	return 0;
}

