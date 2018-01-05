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
		TRACE(_T("Connected\n"));
		while (islistenstatus)
		{
			memset(recvbuf, 0, BUFLENGTH);
			if ((recv_length = recv(currentclientsoc, recvbuf, BUFLENGTH, 0)) < 0)
			{
				
				TRACE(_T("Close client\n"));
				break;			
			}
			else
			{
				std::string strName;
				Json::Value val;
				Json::Reader reader;

				TRACE(_T("recv_length : %d\n"), recv_length);

				TRACE(_T("recv[0] : %c\n"), recvbuf[0]);

				if (recvbuf[0] == 'Q')break;
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