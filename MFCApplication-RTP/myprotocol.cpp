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
	 InitProtocolData();
}

JProtocol::~JProtocol()
{
	CloseHandle(ondata_locker);
	CloseHandle(listen_thread_handle);
	CloseHandle(parse_thread_handle);
	CloseHandle(data_thread_handle);
	/*if (jqueue != NULL)
	{
		delete jqueue;
		jqueue = NULL;
	}*/
}

void JProtocol::InitProtocolData()
{
	socketopen = false;
	serversoc = 0;;
	islistenstatus = false;
	listen_thread_handle = NULL;
	parse_thread_handle = NULL;
	data_thread_handle = NULL;
	memset(recvbuf, 0x00, BUFLENGTH);

	//jqueue = new FifoQueue;
	ondata_locker = CreateMutex(nullptr, FALSE, (LPCWSTR)"ondata");

	//inset states
	statemap.insert(std::pair<string, int>("Connect", CONNECT));
	statemap.insert(std::pair<string, int>("Listening", LISTENING));
	statemap.insert(std::pair<string, int>("Query", QUERY));
	statemap.insert(std::pair<string, int>("CallRequest", CALLREQUEST));
	statemap.insert(std::pair<string, int>("CallRelease", CALLRELEASE));
	statemap.insert(std::pair<string, int>("CallStart", CALLSTART));
	statemap.insert(std::pair<string, int>("CallEND", CALLEND));

	//init default protocol data
	thePROTOCOL_Ctrlr.identifier = "";
	thePROTOCOL_Ctrlr.MASTER_State = CONNECT;
	thePROTOCOL_Ctrlr.type = "Request";
	thePROTOCOL_Ctrlr.name = "Connect";
	thePROTOCOL_Ctrlr.PROTOCOL_params.key = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.reason = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel1_value = 55;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel2_value = 77;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.reason = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.channel_value = 55;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.reason = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.channel_value = 77;
	thePROTOCOL_Ctrlr.PROTOCOL_params.src = 9;
	thePROTOCOL_Ctrlr.PROTOCOL_params.dst = 65536;
	thePROTOCOL_Ctrlr.PROTOCOL_params.channel = "channel1";

}
void JProtocol::CloseMater()
{
	if (socketopen)CloseSocket(serversoc);
	socketopen = false;
	//delete jqueue;
	//jqueue = NULL;
	TRACE(_T("Close Server\n"));

}
void JProtocol::SetCallBackFunc(void(*callBackFunc)(int, ResponeData))
{
	RequestCallBackFunc = callBackFunc;//回调设置
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
	socketopen = true;

	//CreatDataProcessThread();
	CreatProtocolParseThread();
	CreateListenThread();

	/*socketopen = true;*/

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
void JProtocol::CreatProtocolParseThread()
{
	parse_thread_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void *))ProtocolParseThread, this, 0, NULL);
	if (parse_thread_handle == NULL)
	{
		std::cout << "create thread failed" << std::endl;
		system("pause");
	}

}

void JProtocol::DataProcessFunc()
{

	switch (statemap.find(thePROTOCOL_Ctrlr.name)->second)
	{
	case CONNECT:
			TRACE(_T("the ThirdParty Request Connect\n"));
		break;

	case LISTENING:
			TRACE(_T("the ThirdParty Request Listening\n"));
		break;

	case QUERY:
			TRACE(_T("the ThirdParty Request Query\n"));
		break;

	case CALLREQUEST:
			TRACE(_T("the ThirdParty Request Call Start\n"));
		break;
	case CALLRELEASE:
			TRACE(_T("the ThirdParty Request Call Release\n"));
		break;
	case CALLSTART:
	case CALLEND:
			TRACE(_T("no support cmd\n"));
		break;
	default:
		break;
	}

	if (RequestCallBackFunc != NULL)
	{
		ResponeData r = { thePROTOCOL_Ctrlr.identifier, thePROTOCOL_Ctrlr.PROTOCOL_params.key,
			"", -1, -1, -1, -1, "", "" };

		onData(RequestCallBackFunc, statemap.find(thePROTOCOL_Ctrlr.name)->second, r);//callback (*func)
	}


}
int JProtocol::ProtocolParseThread(void *p)
{
	JProtocol *arg = (JProtocol*)p;
	if (arg != NULL)
	{
		arg->ProtocolParseThreadFunc();
	}
	return 0;
}
void JProtocol::ProtocolParseThreadFunc()
{
	std::string str_identifier;
	std::string str_type;
	std::string str_name;
	std::string str_status;
	Json::Value val;
	Json::Reader reader;

	char queue_data[512];
	int len=0;
	int32_t return_value = -1;
	memset(queue_data, 0x00, 512);

	while ((return_value = jqueue.TakeFromQueue(queue_data, (int&)len, 200)) != WAIT_FAILED)//200ms
	{
		if (WAIT_OBJECT_0 == return_value)
		{
			if (reader.parse(queue_data, val))//Parse JSON buff
			{
				thePROTOCOL_Ctrlr.identifier = val["identifier"].asString();
				thePROTOCOL_Ctrlr.type = val["type"].asString();
				thePROTOCOL_Ctrlr.name = val["name"].asString();

				if (thePROTOCOL_Ctrlr.name == "Connect")
					thePROTOCOL_Ctrlr.PROTOCOL_params.key = val["key"].asString();
				else if (thePROTOCOL_Ctrlr.name == "Listening")
				{
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel1_value = val["param"]["listening"]["channel1"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel2_value = val["param"]["listening"]["channel2"].asInt();
				}
				else if (thePROTOCOL_Ctrlr.name == "Query")
				{
					//no param data
				}
				else if ((thePROTOCOL_Ctrlr.name == "CallRequest") || (thePROTOCOL_Ctrlr.name == "CallRelease")
					|| (thePROTOCOL_Ctrlr.name == "CallStart") || (thePROTOCOL_Ctrlr.name == "CallEnd"))
				{
					thePROTOCOL_Ctrlr.PROTOCOL_params.src = val["param"]["src"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.dst = val["param"]["dst"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.channel = val["param"]["channel"].asString();
	
				}
				else
				{
					//other data

				}

				if (thePROTOCOL_Ctrlr.type == "Request")
					DataProcessFunc();//process thePROTOCOL_Ctrlr data
			}
			else
			{
				TRACE(_T("reader.parse err!!!\n"));
			}
			memset(queue_data, 0x00, 512);
			
		}
		else
		{
			//Sleep(200);//200ms
		}

	}
	TRACE(_T(" exit ProtocolParseThreadFunc\n"));
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
		islistenstatus = true;
		ProcessClient(currentclientsoc);//while()
	}
	
	CloseSocket(currentclientsoc);
	islistenstatus = false;
	TRACE(_T("Close socket and exit listenthread\n"));



}

void JProtocol::ProcessClient(SOCKET clientfd)
{

	int recvTimeout = 30 * 1000;   //30s
	int recv_length = 0;
	static int32_t bytes_remained = 0;
	static uint32_t count = 0;
	static uint32_t pro_length = 0;

	//currentclientsoc = clientsoc;
	//设置RECV超时
	setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeout, sizeof(int));
	TRACE(_T("Connected\n"));
	while (islistenstatus)
	{
		if ((recv_length = recv(clientfd, &recvbuf[count], BUFLENGTH, 0)) > 0)
		{
			if ((recvbuf[0] == 'Q') && recv_length == 1)break;

			if ((recvbuf[0] == 0x31) && (recv_length >= 5) && (bytes_remained == 0))
			//if ((recvbuf[0] == 0x01) && (recv_length >= 5) && (bytes_remained ==0))//protocol start
			{
				pro_length = (recvbuf[1] - 0x30) * 1000 + (recvbuf[2] - 0x30) * 100
					+ (recvbuf[3] - 0x30) * 10 + (recvbuf[4] - 0x30);

				bytes_remained = pro_length -(recv_length-5);
				if (bytes_remained == 0)
				{
					jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff
					//clear temp
					pro_length = 0;
					count = 0;
					memset(recvbuf, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
				}
				else if (bytes_remained > 0)
				{
					count += recv_length;
					continue;//wait the 
					 
				}
				else//recv_length >(pro_length+5) (bytes_remained < 0)
				{
					jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff

					memcpy_s(&recvbuf[0], BUFLENGTH, &recvbuf[pro_length+5], recv_length - 5 - pro_length);
					if (recvbuf[0] != 0x01)
					{
						memset(recvbuf, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
						continue;//throw the buff
					}
					count += (recv_length - 5 - pro_length);
					if (count >= 5)
					{
						pro_length = (recvbuf[1] - 0x30) * 1000 + (recvbuf[2] - 0x30) * 100
							+ (recvbuf[3] - 0x30) * 10 + (recvbuf[4] - 0x30);

						bytes_remained = pro_length - (recv_length - 5);
					}
					else
					{
						TRACE(_T("bytes_remained err !!!\n"));
					}
				}

			}
			else if (bytes_remained >0)
			{
				bytes_remained = pro_length - recv_length;
				if (bytes_remained == 0)
				{
					jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff
					//clear temp
					pro_length = 0;
					count = 0;
					memset(recvbuf, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
				}
				else//bytes_remained > 0
				{
					count += recv_length;
					continue;//wait the 
				}

			}
			else//bytes_remained < 0
			{
				TRACE(_T("no happen!!!\n"));
			}


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
		//memset(recvbuf, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];

	}

}

bool JProtocol::CloseSocket(SOCKET sockfd)
{
	closesocket(sockfd);
	return true;

}