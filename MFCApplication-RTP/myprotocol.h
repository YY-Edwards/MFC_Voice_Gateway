
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
	void ConnectReply(std::string status, std::string reason="");

	/*
	���ûظ�
	*/


	
	void ConfigReply(int channel1_value, int channel2_value);

	/*
	��ѯ�ظ�
	*/

	void QueryReply(int channel1_value, int channel2_value);

	/*
	����������ظ�
	*/
	void CallRequestReply(uint32_t src, uint32_t dst, std::string channel );

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
	int SendDataToTheThirdParty(std::string buff);

	/*
	socket��ʼ��
	*/
	bool socketopen;
	//bool islistenstatus;
	bool InitSocket();
	bool CloseSocket(SOCKET sockfd);
	void InitProtocolData();

	HANDLE ondata_locker;
	HANDLE clientmap_locker;
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
	//Json::Value send_root;
	//Json::Value send_arrayObj;
	//Json::Value send_item;
	//Json::StyledWriter style_write;

	std::string CreateGuid();

};
















#endif