
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
	TCP_CMD 协议解析开始
	*/
	void Start();
	
	/*
	回调接口
	设置回调函数
	*/
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));

	/*
	连接回复:
	*/
	void ConnectReply(HSocket fd, std::string status, std::string reason = "");

	/*
	配置回复
	*/
	void ConfigReply(HSocket fd, int channel1_value, int channel2_value);

	/*
	查询回复
	*/

	void QueryReply(HSocket fd, int channel1_value, int channel2_value);

	/*
	请求发起组呼回复
	*/
	void CallRequestReply(HSocket fd, std::string status, std::string reason);

	/*
	结束组呼回复
	*/
	void CallReleaseReply(HSocket fd, std::string status, std::string reason);

	/*
	组呼开始通知
	*/
	void CallStartNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	组呼结束通知
	*/
	void CallEndNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	关闭服务器端
	*/
	void CloseMater();

	/*
	获取Master初始化是否完成
	*/
	bool IsMaterInitComplete()
	{
		return startfunc_is_finished;
	}

private:

	HSocket serversoc;
	struct sockaddr_in my_addr; /* loacl */
	//struct sockaddr_in remote_addr; //client_address
	//socket初始化变量结构体
	socketoption_t socketoption;
	/*
	声明socket接口类
	*/
	CSockWrap *mytcp_server;


	//回调接口
	void(*RequestCallBackFunc)(int, ResponeData);//请求类回调
	//void(*NotifyCallBackFunc)(int, ResponeData);//通知类回调
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
	打包json数据包
	*/
	int SendDataToTheThirdParty(HSocket fd, std::string buff);

	/*
	socket初始化
	*/
	bool socketopen;
	//bool islistenstatus;
	bool InitSocket();
	bool CloseSocket(HSocket sockfd);

	/*
	初始化成员变量
	*/
	void InitProtocolData();

	//HANDLE ondata_locker;
	//HANDLE clientmap_locker;
	ILock *ondata_locker;
	ILock *clientmap_locker;
	//HSocket currentclientsoc;

	//HANDLE listen_thread_handle;
	//线程接口类指针
	MyCreateThread *listen_thread_p[MAX_LISTENING_COUNT];
	MyCreateThread *parse_thread_p;
	//MyCreateThread *data_thread_p;
	//HANDLE parse_thread_handle;
	//HANDLE data_thread_handle;

	char recvbuf[BUFLENGTH];

	FifoQueue jqueue;//JSON data queue

	PROTOCOL_Ctrlr thePROTOCOL_Ctrlr;//协议结构
	std::map <std::string, int>  statemap;//状态机
	std::map <HSocket, struct sockaddr_in>  clientmap;//save client-info

	bool set_thread_exit_flag;
	/*bool listen_thread_exited_flag;
	bool parse_thread_exited_flag;*/
	int listen_numb;//监听计数值

	/*
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }
	bool startfunc_is_finished;

	/*
	获取UUID
	*/
	std::string CreateGuid();
	
	/*
	打包协议数据
	*/
	int PushRecvBuffToQueue(HSocket clientfd, char *buff, int buff_len);
	
	/*
	物理层发送协议数据包
	*/
	int PhySocketSendData(HSocket fd, char *buff, int buff_len);

};
















#endif