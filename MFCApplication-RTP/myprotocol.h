#ifndef MYPROTOCOL_H
#include "Common.h"
#include "myrtp.h"

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
	//void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));

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

	static int ListenThread(void* p);
	//static UINT OnPollThread(LPVOID p);
	void ListenThreadFunc();
	void CreateListenThread();
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
	char recvbuf[BUFLENGTH];

};
















#endif