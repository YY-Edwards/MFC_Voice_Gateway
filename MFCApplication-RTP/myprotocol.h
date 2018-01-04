#ifndef MYPROTOCOL_H

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
	�Ͽ�����
	*/
	void DisConnect();



private:

	/*
	socket��ʼ��
	*/
	bool socketopen;
	bool InitSocket();
	bool CloseSocket(int sockfd);

};
















#endif