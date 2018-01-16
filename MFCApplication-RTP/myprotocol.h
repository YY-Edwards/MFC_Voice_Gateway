
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
	���ӻظ�:
	*/
	void ConnectReply(SOCKET fd, std::string status, std::string reason = "");

	/*
	���ûظ�
	*/
	void ConfigReply(SOCKET fd, int channel1_value, int channel2_value);

	/*
	��ѯ�ظ�
	*/

	void QueryReply(SOCKET fd, int channel1_value, int channel2_value);

	/*
	����������ظ�
	*/
	void CallRequestReply(SOCKET fd, std::string status, std::string reason);

	/*
	��������ظ�
	*/
	void CallReleaseReply(SOCKET fd, std::string status, std::string reason);

	/*
	�����ʼ֪ͨ
	*/
	void CallStartNotify(SOCKET fd, int src, int dst, std::string channel);

	/*
	�������֪ͨ
	*/
	void CallEndNotify(SOCKET fd, int src, int dst, std::string channel);

	/*
	�رշ�������
	*/
	void CloseMater();

	/*
	��ȡMaster��ʼ���Ƿ����
	*/
	bool IsMaterInitComplete()
	{
		return startfunc_is_finished;
	}

private:

	//�ص��ӿ�
	void(*RequestCallBackFunc)(int, ResponeData);//������ص�
	//void(*NotifyCallBackFunc)(int, ResponeData);//֪ͨ��ص�
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);

	int ProcessClient(SOCKET clientfd);
	void CreateListenThread();
	void CreatProtocolParseThread();
	//void CreatDataProcessThread();

	static int ListenThread(void* p);
	int ListenThreadFunc();
	static int ProtocolParseThread(void *p);
	int ProtocolParseThreadFunc();
	//static int  DataProcessThread(void *p);
	//void DataProcessThreadFunc();

	void DataProcessFunc();
	//void WriteJsonData();
	int SendDataToTheThirdParty(SOCKET fd, std::string buff);

	/*
	socket��ʼ��
	*/
	bool socketopen;
	//bool islistenstatus;
	bool InitSocket();
	bool CloseSocket(SOCKET sockfd);
	void InitProtocolData();

	//HANDLE ondata_locker;
	//HANDLE clientmap_locker;
	ILock *ondata_locker;
	ILock *clientmap_locker;
	//SOCKET currentclientsoc;
	SOCKET serversoc;
	struct sockaddr_in my_addr; /* loacl */
	struct sockaddr_in remote_addr; //client_address

	HANDLE listen_thread_handle;
	HANDLE parse_thread_handle;
	HANDLE data_thread_handle;

	char recvbuf[BUFLENGTH];

	FifoQueue jqueue;//JSON data queue

	PROTOCOL_Ctrlr thePROTOCOL_Ctrlr;
	std::map <std::string, int>  statemap;
	std::map <SOCKET, struct sockaddr_in>  clientmap;//save client-info

	bool set_thread_exit_flag;
	bool listen_thread_exited_flag;
	bool parse_thread_exited_flag;

	/*
	�����߳��˳���־
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }

	/*
	��ȡ�����߳��Ƿ��˳�
	*/
	bool IsListenThreadHasExit() 
	{
		if (listen_thread_handle)return listen_thread_exited_flag;
		else
			return true;

	}

	/*
	��ȡ�����߳��Ƿ��˳�
	*/
	bool IsParseThreadHasExit()
	{
		if (parse_thread_handle)return parse_thread_exited_flag;
		else
			return true;

	}

	bool startfunc_is_finished;

	std::string CreateGuid();
	int PushRecvBuffToQueue(SOCKET clientfd, char *buff, int buff_len);
	int PhySocketSendData(SOCKET fd, char *buff, int buff_len);

};
















#endif