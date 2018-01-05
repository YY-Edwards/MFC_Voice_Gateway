
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
	TCP_CMD 协议解析开始
	*/
	void Start();
	
	/*
	回调接口
	设置回调函数
	*/
	void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));

	/*
	连接回复
	*/
	void ConnectReply();

	/*
	配置回复
	*/
	
	void ConfigReply();

	/*
	查询回复
	*/

	void QueryReply();

	/*
	请求发起组呼回复
	*/
	void CallRequestReply();

	/*
	结束组呼回复
	*/
	void CallReleaseReply();

	/*
	组呼开始通知
	*/
	void CallStartNotify();

	/*
	组呼结束通知
	*/
	void CallEndNotify();

	/*
	关闭服务器端
	*/
	void CloseMater();



private:

	HANDLE ondata_locker;
	//回调接口
	void(*RequestCallBackFunc)(int, ResponeData);//请求类回调
	//void(*NotifyCallBackFunc)(int, ResponeData);//通知类回调
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);

	static int ListenThread(void* p);
	void ListenThreadFunc();
	static int ProtocolParseThread(void *p);
	void ProtocolParseThreadFunc();

	void ProcessClient(SOCKET clientfd);
	void CreateListenThread();
	void CreatProtocolParseThread();
	/*
	socket初始化
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