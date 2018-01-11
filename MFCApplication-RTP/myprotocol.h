
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
	void ConnectReply(std::string status, std::string reason="");

	/*
	配置回复
	*/


	
	void ConfigReply(int channel1_value, int channel2_value);

	/*
	查询回复
	*/

	void QueryReply(int channel1_value, int channel2_value);

	/*
	请求发起组呼回复
	*/
	void CallRequestReply(std::string status, std::string reason);

	/*
	结束组呼回复
	*/
	void CallReleaseReply(std::string status, std::string reason);

	/*
	组呼开始通知
	*/
	void CallStartNotify(int src, int dst, std::string channel);

	/*
	组呼结束通知
	*/
	void CallEndNotify(int src, int dst, std::string channel);

	/*
	关闭服务器端
	*/
	void CloseMater();

private:

	//回调接口
	void(*RequestCallBackFunc)(int, ResponeData);//请求类回调
	//void(*NotifyCallBackFunc)(int, ResponeData);//通知类回调
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
	socket初始化
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
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }

	/*
	获取监听线程是否退出
	*/
	bool IsListenThreadHasExit() 
	{
		if (listen_thread_handle)return listen_thread_exited_flag;
		else
			return true;

	}

	/*
	获取解析线程是否退出
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