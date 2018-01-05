/*
* myprotocol.cpp
*
* Created: 2018/01/03
* Author: EDWARDS
*/


#include "stdafx.h"
#include "myprotocol.h"

JProtocol::JProtocol()
{

	 socketopen =false;
	 serversoc = 0;;
	 islistenstatus = true;
	 listen_thread_handle = NULL;

	 ondata_locker = CreateMutex(nullptr, FALSE, (LPCWSTR)"ondata");

}

JProtocol::~JProtocol()
{
	CloseHandle(ondata_locker);
	CloseHandle(listen_thread_handle);
}
void JProtocol::CloseMater()
{
	if (socketopen)CloseSocket(serversoc);
	socketopen = false;
	TRACE(_T("Close Server\n"));

}


void JProtocol::SetCallBackFunc(void(*callBackFunc)(int, ResponeData))
{
	RequestCallBackFunc = callBackFunc;
}

void JProtocol::onData(void(*func)(int, ResponeData), int command, ResponeData data)
{
	WaitForSingleObject(ondata_locker, INFINITE);
	try
	{
		func(command, data);
	}
	catch (double)
	{
		TRACE(_T("func error...\n"));

	}
	ReleaseMutex(ondata_locker);

}

void JProtocol::Start()
{
	bool status = false;
	status = InitSocket();
	if (status != true)
	{
		TRACE(_T("InitSocket fail...\n"));
	}

}

bool JProtocol::InitSocket()
{
	if (socketopen)
	{
		CloseSocket(serversoc);
	}

	if ((serversoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) //create a tcp socket  
	{
		TRACE(_T("Create socket fail!\n"));
		//printf("Create socket fail!\n");
		return false;
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(TCPPORT);
	my_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	//将本地地址绑定到所创建的套接字上
	if (bind(serversoc, (LPSOCKADDR)&my_addr, sizeof(my_addr)) == SOCKET_ERROR)
	{
		CloseSocket(serversoc);
		return false;
	}

	//start listen
	if (listen(serversoc, 5) != 0)
	{
		CloseSocket(serversoc);
		return false;
	}

	CreateListenThread();

	socketopen = true;

	//listen_thread_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void *))ListenThread, this, 0, NULL);
	//if (listen_thread_handle == NULL)
	//{
	//	std::cout << "create thread failed" << std::endl;
	//	system("pause");
	//}


	return true;

}

void JProtocol::CreateListenThread()
{
	listen_thread_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void *))ListenThread, this, 0, NULL);
	if (listen_thread_handle == NULL)
	{
		std::cout << "create thread failed" << std::endl;
		system("pause");
	}

}

int JProtocol::ListenThread(void* p)
{
	JProtocol *arg = (JProtocol*)p;
	if (arg != NULL)
	{
		arg->ListenThreadFunc();
	}
	return 0;
}

void JProtocol::ListenThreadFunc()
{
	int sin_size = 0;
	std::string str_identifier;
	int recvTimeout = 10 * 1000;   //10s
	std::string str_type;
	std::string str_name;

	Json::Value val;
	Json::Reader reader;
	sin_size = sizeof(struct sockaddr_in);
	SOCKET currentclientsoc;
	int recv_length = 0;

	if ((currentclientsoc = accept(serversoc, (sockaddr*)&remote_addr, &sin_size)) < 0)
	{
		TRACE(_T("accept fail!\n"));
	}
	if (socketopen)CreateListenThread();//服务器运行中则继续监听

	if (currentclientsoc == INVALID_SOCKET)
	{
		TRACE(_T("Connect fail\n"));
	}
	else
	{
		//currentclientsoc = clientsoc;
		//设置RECV超时
		setsockopt(currentclientsoc, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeout, sizeof(int));
		TRACE(_T("Connected\n"));
		while (islistenstatus)
		{
			memset(recvbuf, 0, BUFLENGTH);
			if ((recv_length = recv(currentclientsoc, recvbuf, BUFLENGTH, 0)) > 0)
			{
				if (reader.parse(recvbuf, val))
				{

					str_type = val["type"].asString();
					TRACE(("type : %s\n"), str_type.c_str());

					str_name = val["name"].asString();
					TRACE(("name : %s\n"), str_name.c_str());

					str_identifier = val["identifier"].asString();
					TRACE(("identifier : %s\n"), str_identifier.c_str());

				}
				else
				{
					TRACE(_T("reader.parse err!!!\n"));
				}

				/*	TRACE(_T("recv_length : %d\n"), recv_length);

				TRACE(_T("recv[0] : %c\n"), recvbuf[0]);*/

				if (recvbuf[0] == 'Q')break;
				
			}
			else
			{
				if ((recv_length < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == 0))
				{
					if (errno == 0)
					{
						TRACE(_T("Timeout\n"));
					}
					continue;
				}
				else
				{

					TRACE(_T("Close client\n"));
					break;
				}
				
			}
			
		}
	}
	
	CloseSocket(currentclientsoc);
	TRACE(_T("Close socket and exit listenthread\n"));



}

bool JProtocol::CloseSocket(SOCKET sockfd)
{
	closesocket(sockfd);
	return true;

}