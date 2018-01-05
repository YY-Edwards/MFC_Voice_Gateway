
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
#include "JsonQueue.h"

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

	HANDLE ondata_locker;
	//�ص��ӿ�
	void(*RequestCallBackFunc)(int, ResponeData);//������ص�
	//void(*NotifyCallBackFunc)(int, ResponeData);//֪ͨ��ص�
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);

	static int ListenThread(void* p);
	void ListenThreadFunc();
	static int ProtocolParseThread(void *p);
	void ProtocolParseThreadFunc();

	void ProcessClient(SOCKET clientfd);
	void CreateListenThread();
	void CreatProtocolParseThread();
	/*
	socket��ʼ��
	*/
	bool socketopen;
	bool islistenstatus;
	bool InitSocket();
	bool CloseSocket(SOCKET sockfd);
	SOCKET serversoc;
	//SOCKET clientsoc;
	struct sockaddr_in my_addr; /* loacl */
	struct sockaddr_in remote_addr; //client_address
	HANDLE listen_thread_handle;
	HANDLE parse_thread_handle;
	char recvbuf[BUFLENGTH];
	JsonQueue *jqueue;

};
















#endif