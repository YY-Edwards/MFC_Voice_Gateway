
/*
* myprotocol.h
*
* Created: 2018/01/03
* Author: EDWARDS
*/


#ifndef MYPROTOCOL_H
#include "Common.h"
#include "myrtp.h"
#include "json.h"
#include "FifoQueue.h"

class JProtocol
{
public:
	JProtocol();
	~JProtocol();


	/*
	TCP_CMD Э�������ʼ
	*/
	void Start();
	
	/*
	�ص��ӿ�
	���ûص�����
	*/
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));

	/*
	���ӻظ�
	*/
	void ConnectReply();

	/*
	���ûظ�
	*/
	
	void ConfigReply();

	/*
	��ѯ�ظ�
	*/

	void QueryReply();

	/*
	����������ظ�
	*/
	void CallRequestReply();

	/*
	��������ظ�
	*/
	void CallReleaseReply();

	/*
	�����ʼ֪ͨ
	*/
	void CallStartNotify();

	/*
	�������֪ͨ
	*/
	void CallEndNotify();

	/*
	�رշ�������
	*/
	void CloseMater();



private:

	//�ص��ӿ�
	void(*RequestCallBackFunc)(int, ResponeData);//������ص�
	//void(*NotifyCallBackFunc)(int, ResponeData);//֪ͨ��ص�
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);

	void ProcessClient(SOCKET clientfd);
	void CreateListenThread();
	void CreatProtocolParseThread();
	//void CreatDataProcessThread();

	static int ListenThread(void* p);
	void ListenThreadFunc();
	static int ProtocolParseThread(void *p);
	void ProtocolParseThreadFunc();
	//static int  DataProcessThread(void *p);
	//void DataProcessThreadFunc();

	void DataProcessFunc();

	/*
	socket��ʼ��
	*/
	bool socketopen;
	bool islistenstatus;
	bool InitSocket();
	bool CloseSocket(SOCKET sockfd);
	void InitProtocolData();

	HANDLE ondata_locker;
	SOCKET serversoc;
	//SOCKET clientsoc;
	struct sockaddr_in my_addr; /* loacl */
	struct sockaddr_in remote_addr; //client_address

	HANDLE listen_thread_handle;
	HANDLE parse_thread_handle;
	HANDLE data_thread_handle;

	char recvbuf[BUFLENGTH];
	//FifoQueue *jqueue;
	FifoQueue jqueue;//JSON data queue
	//FifoQueue dqueue;//PROTOCOL_Ctrlr data queue
	PROTOCOL_Ctrlr thePROTOCOL_Ctrlr;
	std::map <std::string, int>  statemap;

};
















#endif