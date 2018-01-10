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
	
	CloseMater();
	//WSACleanup();
	SetThreadExitFlag();
	do
	{
		Sleep(30);
	} while ((!(IsListenThreadHasExit())) || (!(IsParseThreadHasExit())));

	CloseHandle(ondata_locker);
	CloseHandle(listen_thread_handle);
	CloseHandle(parse_thread_handle);
	CloseHandle(data_thread_handle);
	CloseHandle(clientmap_locker);
	jqueue.ClearQueue();
	statemap.clear();
	clientmap.clear();

	WSACleanup();
	TRACE(_T("Destory: JProtocol \n"));


	/*if (jqueue != NULL)
	{
		delete jqueue;
		jqueue = NULL;
	}*/
}

void JProtocol::InitProtocolData()
{
	
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);//init socket

	set_thread_exit_flag = false;
	listen_thread_exited_flag = false;
	parse_thread_exited_flag = false;

	socketopen = false;
	serversoc = 0;;
	//islistenstatus = false;
	listen_thread_handle = NULL;
	parse_thread_handle = NULL;
	data_thread_handle = NULL;
	memset(recvbuf, 0x00, BUFLENGTH);
	RequestCallBackFunc = NULL;

	//jqueue = new FifoQueue;
	ondata_locker = CreateMutex(nullptr, FALSE, (LPCWSTR)"ondata");
	clientmap_locker = CreateMutex(nullptr, FALSE, (LPCWSTR)"clientmap");

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
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel1_value = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel2_value = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.reason = "timeout";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.channel_value = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.reason = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.channel_value = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.src = 9;
	thePROTOCOL_Ctrlr.PROTOCOL_params.dst = 65536;
	thePROTOCOL_Ctrlr.PROTOCOL_params.channel = "";

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
		thePROTOCOL_Ctrlr.PROTOCOL_params.channel, thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.channel_value,
		thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.channel_value,
		thePROTOCOL_Ctrlr.PROTOCOL_params.src, thePROTOCOL_Ctrlr.PROTOCOL_params.dst,
		"", "" };

		onData(RequestCallBackFunc, statemap.find(thePROTOCOL_Ctrlr.name)->second, r);//callback (*func)
	}


}
int JProtocol::ProtocolParseThread(void *p)
{
	JProtocol *arg = (JProtocol*)p;
	int return_value = 0;
	if (arg != NULL)
	{
		return_value = arg->ProtocolParseThreadFunc();
	}
	return return_value;
}
int JProtocol::ProtocolParseThreadFunc()
{
	/*std::string str_identifier;
	std::string str_type;
	std::string str_name;
	std::string str_status;*/
	Json::Value val;
	Json::Reader reader;

	char queue_data[512];
	int len=0;
	int return_value = -1;
	memset(queue_data, 0x00, 512);

	while ((return_value = jqueue.TakeFromQueue(queue_data, (int&)len, 200)) != WAIT_FAILED)//200ms
	{
		if (set_thread_exit_flag)break;
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
					thePROTOCOL_Ctrlr.PROTOCOL_params.Channel1_Param.channel_value = thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel1_value;
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel2_value = val["param"]["listening"]["channel2"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.Channel2_Param.channel_value = thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_channles.channel2_value;

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
			val.clear();
			memset(queue_data, 0x00, 512);		
		}
		else
		{
			//Sleep(200);//200ms
		}

	}

	TRACE((" exit ProtocolParseThreadFunc : 0x%x\n"), GetCurrentThreadId());
	parse_thread_exited_flag = true;
	return return_value;
}
int JProtocol::ListenThread(void* p)
{
	JProtocol *arg = (JProtocol*)p;
	int return_value = 0;
	if (arg != NULL)
	{
		return_value = arg->ListenThreadFunc();
	}
	return return_value;
}
int JProtocol::ListenThreadFunc()
{
	int sin_size = 0;
	int return_value = 0;
	sin_size = sizeof(struct sockaddr_in);
	SOCKET currentclientsoc;
	
	TRACE(_T("The server is waiting for the connection \n"));
	currentclientsoc = accept(serversoc, (sockaddr*)&remote_addr, &sin_size);

	if (socketopen)CreateListenThread();//服务器运行中则继续监听

	if (currentclientsoc == INVALID_SOCKET)
	{
		TRACE(_T("accept fail!\n"));
		return_value = -1;
	}
	else
	{
		WaitForSingleObject(clientmap_locker, INFINITE);
		clientmap.insert(std::pair<SOCKET, struct sockaddr_in>(currentclientsoc, remote_addr));//save client info(socketfd,ip,port...) to map
		ReleaseMutex(clientmap_locker);
		return_value = ProcessClient(currentclientsoc);//while(1)
	}
	
	WaitForSingleObject(clientmap_locker, INFINITE);
	clientmap.erase(currentclientsoc);
	ReleaseMutex(clientmap_locker);

	CloseSocket(currentclientsoc);
	TRACE(("Erase clientfd ,Close socket and exit listenthread: 0x%x\n"), GetCurrentThreadId());
	listen_thread_exited_flag = true;

	return return_value;

}

int JProtocol::ProcessClient(SOCKET clientfd)
{
	int return_value = 0;
	int recvTimeout = 30 * 1000;   //30s
	int recv_length = 0;
	static int32_t bytes_remained = 0;
	static uint32_t count = 0;
	static uint32_t pro_length = 0;

	//currentclientsoc = clientsoc;
	//设置RECV超时
	setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeout, sizeof(int));
	TRACE(_T("Connected\n"));
	//while (islistenstatus)
	while (!set_thread_exit_flag)
	{
		if ((recv_length = recv(clientfd, &recvbuf[count], BUFLENGTH, 0)) > 0)
		{
			if ((recvbuf[0] == 'Q') && recv_length == 1)
			{
				return_value = 0;
				break;
			}

		Start:

			//if ((recvbuf[0] == 0x31) && (recv_length >= 5) && (bytes_remained == 0))
			if ((recvbuf[0] == PROTOCOL_HEAD) && (recv_length >= 5) && (bytes_remained == 0))//protocol start
			{

					pro_length = (recvbuf[1] - 0x30) * 1000 + (recvbuf[2] - 0x30) * 100
						+ (recvbuf[3] - 0x30) * 10 + (recvbuf[4] - 0x30);

					if (recvbuf[5] != '{')
					{
						pro_length = 0;
						count = 0;
						memset(recvbuf, 0, BUFLENGTH);
						continue;
					}
					bytes_remained = pro_length -(recv_length-5);

					if (bytes_remained == 0)
					{
						TRACE(_T("recv_okay\n"));
						jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff
						//clear temp
						pro_length = 0;
						count = 0;
						memset(recvbuf, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
					}
					else if (bytes_remained > 0)//need  to stick the buff
					{
						TRACE(_T("need to stick the buff\n"));
						count += recv_length;
						//continue;//wait the 
					 
					}
					else//recv_length >(pro_length+5) (bytes_remained < 0)//need  to dismantle the buff
					{
						TRACE(_T("need  to dismantle the buff\n"));
						jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff

						memcpy_s(&recvbuf[0], BUFLENGTH, &recvbuf[pro_length+5], recv_length - 5 - pro_length);

						recv_length = recv_length - 5 - pro_length;
						bytes_remained = 0;
						count = recv_length;
						goto Start;

					}

			}
			else if (bytes_remained >0)
			{
				if (bytes_remained == recv_length)
				{
					TRACE(_T("recv_okay\n"));
					jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff
					//clear temp
					pro_length = 0;
					count = 0;
					bytes_remained = 0;
					memset(recvbuf, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
				}
				else if (bytes_remained > recv_length)
				{
					TRACE(_T("need to stick the buff-2\n"));
					bytes_remained -= recv_length;
					count += recv_length;
					//continue;//wait the 
				}
				else//bytes_remained < recv_length
				{
					TRACE(_T("need  to dismantle the buff-2\n"));
					jqueue.PushToQueue(&recvbuf[5], pro_length);//push to fifo-buff
					memcpy_s(&recvbuf[0], BUFLENGTH, &recvbuf[pro_length + 5], (count + recv_length - pro_length - 5));

					recv_length = count + recv_length - pro_length - 5;
					bytes_remained = 0;
					count = recv_length;
					goto Start;

				}

			}
			else//bytes_remained < 0
			{
				pro_length = 0;
				count = 0;
				bytes_remained = 0;
				memset(recvbuf, 0, BUFLENGTH);
				TRACE(_T("Recv err data and clear temp!!!\n"));
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
				return_value = -1;
				break;
			}
			
		}
		

	}

	return return_value;
}

bool JProtocol::CloseSocket(SOCKET sockfd)
{
	closesocket(sockfd);
	return true;

}

int JProtocol::SendDataToTheThirdParty(std::string buff)
{
	int copy_len = -1;
	int count = 0;
	int return_value = 0;
	char send_buff[1024];
	char len_buff[4];
	int pro_len = 0;
	int temp_s = 0;
	int temp_r = 0;
	memset(len_buff, 0x00, 4);
	memset(send_buff, 0x00, 1024);

	temp_s = buff.size() / 1000;
	temp_r = buff.size() % 1000;
	len_buff[0] = temp_s + 0x30;//千位

	temp_s = temp_r / 100;
	temp_r = temp_r % 100;
	len_buff[1] = temp_s + 0x30;//百位

	temp_s = temp_r / 10;
	temp_r = temp_r % 10;
	len_buff[2] = temp_s + 0x30;//十位
	len_buff[3] = temp_r + 0x30;//个位

	//build protocol data
	send_buff[0] = PROTOCOL_HEAD;
	memcpy_s(&send_buff[1], 4, len_buff, 4);
	memcpy_s(&send_buff[5], buff.size(), buff.c_str(), buff.size());
	pro_len = 5 + buff.size();

	if (!clientmap.empty())
	{
		SOCKET Objsoc =0;
		WaitForSingleObject(clientmap_locker, INFINITE);
		Objsoc = clientmap.begin()->first;
		ReleaseMutex(clientmap_locker);

		do
		{
			copy_len = send(Objsoc, &send_buff[count], (pro_len-count), 0);
			if (copy_len < 0)
			{
				count = -1;
				TRACE(_T("send SOCKET_ERROR!!!\n"));
				break;
			}
			else
			{
				count += copy_len;
				TRACE(("send length is %d\n"), copy_len);
			}

		} while ((pro_len-count) != 0);

	}
	else
	{
		TRACE(_T("clientmap is empty!!!\n"));
	}

	count = 0;
	return count;
}

void JProtocol::ConnectReply(std::string status, std::string reason)
{
	
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = "2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "Connect";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(SendBuf);
	//SendBuf.clear();
	//send_root.clear();
	//send_arrayObj.clear();
	//send_item.clear();
	TRACE(_T("Send ConnectReply\n"));

}

void JProtocol::ConfigReply(int channel1_value, int channel2_value)
{
	Json::Value send_root;
	Json::Value send_arrayObj1;
	Json::Value send_arrayObj2;
	Json::Value send_arrayObj3;
	Json::Value send_item1;
	Json::Value send_item2;
	Json::Value send_item3;
	Json::StyledWriter style_write;

	if (channel1_value != 0)
	{
		send_item1["status"] = "success";
		send_item1["reason"] = "";
	}
	else
	{
		send_item1["status"] = "fail";
		send_item1["reason"] = "unset";
	}
	send_item1["value"] = channel1_value;
	send_arrayObj1.append(send_item1);

	if (channel2_value != 0)
	{
		send_item2["status"] = "success";
		send_item2["reason"] = "";
	}
	else
	{
		send_item2["status"] = "fail";
		send_item2["reason"] = "unset";
	}
	send_item2["value"] = channel2_value;
	send_arrayObj2.append(send_item2);

	send_item3["channel1"] = send_arrayObj1;
	send_item3["channel2"] = send_arrayObj2;
	send_arrayObj3.append(send_item3);


	send_root["identifier"] = "2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "Listening";
	send_root["param"] = send_arrayObj3;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(SendBuf);

	TRACE(_T("Send ConfigReply\n"));


}
void JProtocol::QueryReply()
{
	TRACE(_T("Send QueryReply\n"));

}
void JProtocol::CallRequestReply()
{
	TRACE(_T("Send CallRequestReply\n"));

}
void JProtocol::CallReleaseReply()
{
	TRACE(_T("Send CallReleaseReply\n"));

}
void JProtocol::CallStartNotify()
{
	TRACE(_T("Send CallStartNotify\n"));

}
void JProtocol::CallEndNotify()
{
	TRACE(_T("Send CallEndNotify\n"));

}