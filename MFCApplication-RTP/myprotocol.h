#ifndef MYPROTOCOL_H

#include "myrtp.h"

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
	断开连接
	*/
	void DisConnect();



private:

	/*
	socket初始化
	*/
	bool socketopen;
	bool InitSocket();
	bool CloseSocket(int sockfd);

};
















#endif