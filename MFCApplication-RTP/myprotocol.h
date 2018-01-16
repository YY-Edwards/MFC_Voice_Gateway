
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
#include "socket_wrap.h"

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
	void ConnectReply(HSocket fd, std::string status, std::string reason = "");

	/*
	���ûظ�
	*/
	void ConfigReply(HSocket fd, int channel1_value, int channel2_value);

	/*
	��ѯ�ظ�
	*/

	void QueryReply(HSocket fd, int channel1_value, int channel2_value);

	/*
	����������ظ�
	*/
	void CallRequestReply(HSocket fd, std::string status, std::string reason);

	/*
	��������ظ�
	*/
	void CallReleaseReply(HSocket fd, std::string status, std::string reason);

	/*
	�����ʼ֪ͨ
	*/
	void CallStartNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	�������֪ͨ
	*/
	void CallEndNotify(HSocket fd, int src, int dst, std::string channel);

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

	HSocket serversoc;
	struct sockaddr_in my_addr; /* loacl */
	//struct sockaddr_in remote_addr; //client_address
	//socket��ʼ�������ṹ��
	socketoption_t socketoption;
	/*
	����socket�ӿ���
	*/
	CSockWrap *mytcp_server;


	//�ص��ӿ�
	void(*RequestCallBackFunc)(int, ResponeData);//������ص�
	//void(*NotifyCallBackFunc)(int, ResponeData);//֪ͨ��ص�
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);

	int ProcessClient(HSocket clientfd);
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
	
	/*
	���json���ݰ�
	*/
	int SendDataToTheThirdParty(HSocket fd, std::string buff);

	/*
	socket��ʼ��
	*/
	bool socketopen;
	//bool islistenstatus;
	bool InitSocket();
	bool CloseSocket(HSocket sockfd);

	/*
	��ʼ����Ա����
	*/
	void InitProtocolData();

	//HANDLE ondata_locker;
	//HANDLE clientmap_locker;
	ILock *ondata_locker;
	ILock *clientmap_locker;
	//HSocket currentclientsoc;

	//HANDLE listen_thread_handle;
	//�߳̽ӿ���ָ��
	MyCreateThread *listen_thread_p[MAX_LISTENING_COUNT];
	MyCreateThread *parse_thread_p;
	//MyCreateThread *data_thread_p;
	//HANDLE parse_thread_handle;
	//HANDLE data_thread_handle;

	char recvbuf[BUFLENGTH];

	FifoQueue jqueue;//JSON data queue

	PROTOCOL_Ctrlr thePROTOCOL_Ctrlr;//Э��ṹ
	std::map <std::string, int>  statemap;//״̬��
	std::map <HSocket, struct sockaddr_in>  clientmap;//save client-info

	bool set_thread_exit_flag;
	/*bool listen_thread_exited_flag;
	bool parse_thread_exited_flag;*/
	int listen_numb;//��������ֵ

	/*
	�����߳��˳���־
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }
	bool startfunc_is_finished;

	/*
	��ȡUUID
	*/
	std::string CreateGuid();
	
	/*
	���Э������
	*/
	int PushRecvBuffToQueue(HSocket clientfd, char *buff, int buff_len);
	
	/*
	����㷢��Э�����ݰ�
	*/
	int PhySocketSendData(HSocket fd, char *buff, int buff_len);

};
















#endif