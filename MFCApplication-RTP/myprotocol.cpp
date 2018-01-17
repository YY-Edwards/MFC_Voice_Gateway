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
	SetThreadExitFlag();//通知线程退出

	if (parse_thread_p != NULL)
	{
		delete parse_thread_p;
		parse_thread_p = NULL;

	}
	for (int i = 0; i < MAX_LISTENING_COUNT; i++)
	{
		if (listen_thread_p[i] != NULL)
		{
			delete listen_thread_p[i];
			listen_thread_p[i] = NULL;
		}
	}

	jqueue.ClearQueue();
	statemap.clear();
	clientmap.clear();

	if (ondata_locker != NULL)
	{
		delete ondata_locker;
		ondata_locker = NULL;
	}
	if (clientmap_locker != NULL)
	{
		delete clientmap_locker;
		clientmap_locker = NULL;
	}
	if (stickdismantle_locker != NULL)
	{
		delete stickdismantle_locker;
		stickdismantle_locker = NULL;
	}

	FreeSocketEnvironment();
	TRACE(_T("Destory: JProtocol \n"));

}

void JProtocol::InitProtocolData()
{
	InitializeSocketEnvironment();

	mytcp_server = NULL;
	socketoption.recvtimeout = TIMEOUT_VALUE;//10s
	socketoption.sendtimeout = TIMEOUT_VALUE;//10s
	socketoption.lingertimeout = TIMEOUT_VALUE;
	socketoption.block = TRUE;

	startfunc_is_finished = false;
	set_thread_exit_flag = false;

	socketopen = false;
	serversoc = INVALID_SOCKET;
	listen_numb = 0;
	memset(listen_thread_p, 0, MAX_LISTENING_COUNT*sizeof(MyCreateThread *));//注意大小问题
	parse_thread_p = NULL;

	RequestCallBackFunc = NULL;

	ondata_locker = new CriSection();
	clientmap_locker = new CriSection();
	stickdismantle_locker = new CriSection();

	//inset states
	statemap.insert(std::pair<string, int>("Connect", CONNECT));
	statemap.insert(std::pair<string, int>("Listening", LISTENING));
	statemap.insert(std::pair<string, int>("Query", QUERY));
	statemap.insert(std::pair<string, int>("CallRequest", CALLREQUEST));
	statemap.insert(std::pair<string, int>("CallRelease", CALLRELEASE));
	statemap.insert(std::pair<string, int>("CallStart", CALLSTART));
	statemap.insert(std::pair<string, int>("CallEND", CALLEND));

	//init default protocol data
	thePROTOCOL_Ctrlr.clientfd = INVALID_SOCKET;
	thePROTOCOL_Ctrlr.MASTER_State = CONNECT;
	thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier = "";
	thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.type = "Request";
	thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name = "Connect";
	thePROTOCOL_Ctrlr.PROTOCOL_params.key = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.reason = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id = 0;


	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.reason = "timeout";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.listening_group_id = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.reason = "timeout";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.listening_group_id = 0;

	thePROTOCOL_Ctrlr.PROTOCOL_params.src = 9;
	thePROTOCOL_Ctrlr.PROTOCOL_params.dst = 65536;
	thePROTOCOL_Ctrlr.PROTOCOL_params.channel = "";

}
void JProtocol::CloseMater()
{
	if (serversoc != INVALID_SOCKET)
	{
		serversoc = INVALID_SOCKET;
		delete mytcp_server;
		mytcp_server = NULL;
	}

	TRACE(_T("Close Server\n"));

}
void JProtocol::SetCallBackFunc(void(*callBackFunc)(int, ResponeData))
{
	RequestCallBackFunc = callBackFunc;//回调设置
}
void JProtocol::onData(void(*func)(int, ResponeData), int command, ResponeData data)
{
	//WaitForSingleObject(ondata_locker, INFINITE);
	ondata_locker->Lock();
	try
	{
		func(command, data);
	}
	catch (double)
	{
		TRACE(_T("func error...\n"));

	}
	ondata_locker->Unlock();
	//ReleaseMutex(ondata_locker);

}
void JProtocol::Start()
{
	bool status = false;
	status = InitSocket();
	if (status != true)
	{
		TRACE(_T("InitSocket fail...\n"));
	}
	else
	{
		startfunc_is_finished = status;
	}

}
bool JProtocol::InitSocket()
{
	if (mytcp_server != NULL)
	{
		mytcp_server->Reopen(TRUE);
	}
	else
	{
		mytcp_server = new CSockWrap(SOCK_STREAM);
	}
	serversoc = mytcp_server->GetHandle();//获取服务端描述符
	GetAddressFrom(&my_addr, 0, TCPPORT);//本地任意IP
	if (SocketBind(serversoc, &my_addr) == SOCKET_ERROR)
	{
		delete mytcp_server;
		mytcp_server = NULL;
		serversoc = INVALID_SOCKET;
		return false;
	}
	if (SocketListen(serversoc, 100) != 0)
	{
		delete mytcp_server;
		mytcp_server = NULL;
		serversoc = INVALID_SOCKET;
		return false;
	}


	/*
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

	*/

	//CreatDataProcessThread();
	CreatProtocolParseThread();
	CreateListenThread();

	/*socketopen = true;*/

	return true;

}
void JProtocol::CreateListenThread()
{
	if (listen_numb < MAX_LISTENING_COUNT)
	{
		listen_thread_p[listen_numb++] = new MyCreateThread(ListenThread, this);
	}

}
void JProtocol::CreatProtocolParseThread()
{
	parse_thread_p = new MyCreateThread(ProtocolParseThread, this);

}

void JProtocol::DataProcessFunc()
{

	switch (statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second)
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
		ResponeData r = { thePROTOCOL_Ctrlr.clientfd, thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier, thePROTOCOL_Ctrlr.PROTOCOL_params.key,
		thePROTOCOL_Ctrlr.PROTOCOL_params.channel, thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id,
		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id,
		thePROTOCOL_Ctrlr.PROTOCOL_params.src, thePROTOCOL_Ctrlr.PROTOCOL_params.dst,
		"", "" };

		onData(RequestCallBackFunc, statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second, r);//callback (*func)
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
	Json::Value val;
	Json::Reader reader;
	char queue_data[1024];
	char json_data[512];
	int len=0;
	int return_value = SYN_ABANDONED;
	HSocket client_fd = INVALID_SOCKET;
	memset(queue_data, 0x00, 1024);
	memset(json_data, 0x00, 512);

	while ((return_value = jqueue.TakeFromQueue(queue_data, (int&)len, 200)) >= 0)//200ms
	{
		if (set_thread_exit_flag)break;
		if (SYN_OBJECT_o == return_value)
		{
			sscanf(queue_data, "%4D", &client_fd);
			memcpy(json_data, &queue_data[sizeof(HSocket)], sizeof(json_data));
			if (reader.parse(json_data, val))//Parse JSON buff
			{
				thePROTOCOL_Ctrlr.clientfd = client_fd;

				thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier = val["identifier"].asString();
				thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.type = val["type"].asString();
				thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name = val["name"].asString();

				if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "Connect")
					thePROTOCOL_Ctrlr.PROTOCOL_params.key = val["key"].asString();
				else if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "Listening")
				{
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id = val["param"]["listening"]["channel1"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.listening_group_id = thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id;
					
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id = val["param"]["listening"]["channel2"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.listening_group_id = thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id;

				}
				else if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "Query")
				{
					//no param data
				}
				else if ((thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallRequest") || (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallRelease")
					|| (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallStart") || (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallEnd"))
				{
					thePROTOCOL_Ctrlr.PROTOCOL_params.src = val["param"]["src"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.dst = val["param"]["dst"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.channel = val["param"]["channel"].asString();
	
				}
				else
				{
					//other data

				}

				if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.type == "Request")
					DataProcessFunc();//process thePROTOCOL_Ctrlr data
			}
			else
			{
				TRACE(_T("reader.parse err!!!\n"));
			}
			val.clear();
			client_fd = INVALID_SOCKET;
			memset(queue_data, 0x00, 1024);
			memset(json_data, 0x00, 512);
		}
		else//timeout
		{
			//Sleep(200);//200ms
		}

	}

	TRACE((" exit ProtocolParseThreadFunc : 0x%x\n"), GetCurrentThreadId());
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
	int return_value = 0;
	HSocket currentclientsoc = INVALID_SOCKET;
	struct sockaddr_in remote_addr; //client_address
	
	TRACE(_T("The server is waiting for the connection \n"));
	currentclientsoc = SocketAccept(serversoc, (sockaddr_in*)&remote_addr);

	if (serversoc != INVALID_SOCKET)CreateListenThread();//服务器运行中则继续监听

	if (currentclientsoc == INVALID_SOCKET)
	{
		return_value = -1;
		TRACE(("Accept fail! and exit listenthread: 0x%x\n"), GetCurrentThreadId());
	}
	else
	{
		//WaitForSingleObject(clientmap_locker, INFINITE);
		clientmap_locker->Lock();
		clientmap.insert(std::pair<HSocket, struct sockaddr_in>(currentclientsoc, remote_addr));//save client info(socketfd,ip,port...) to map
		clientmap_locker->Unlock();
		//ReleaseMutex(clientmap_locker);
		return_value = ProcessClient(currentclientsoc);//while(1)
		

		//WaitForSingleObject(clientmap_locker, INFINITE);
		clientmap_locker->Lock();
		clientmap.erase(currentclientsoc);
		clientmap_locker->Unlock();
		//ReleaseMutex(clientmap_locker);

		SocketClose(currentclientsoc);
		//CloseSocket(currentclientsoc);
		TRACE(("Erase clientfd ,Close socket and exit ProcessClientFunc: 0x%x\n"), GetCurrentThreadId());
	}

	return return_value;

}

int JProtocol::PushRecvBuffToQueue(HSocket clientfd, char *buff, int buff_len)
{
	if ((buff_len > 512) || (clientfd == INVALID_SOCKET))return -1;
	char data[2048];
	memset(data, 0x00, sizeof(data));
	/*GOSPRINTF(data, sizeof(HSocket)+1, "%d", clientfd);*/
	sprintf(data, "%d", clientfd);
	memcpy(&data[sizeof(HSocket)], buff, buff_len);
	jqueue.PushToQueue(data, buff_len + sizeof(HSocket));//push to fifo-buff
	return 0;

}
int JProtocol::ProcessClient(HSocket clientfd)
{
	int return_value = 0;
	char recvbuff[BUFLENGTH];
	memset(recvbuff, 0x00, BUFLENGTH);
	transresult_t rt;
	StickDismantleOptions_t temp_option;
	memset(&temp_option, 0x00, sizeof(StickDismantleOptions_t));

	std::string len_str;
	len_str.clear();

	//设置clientfd超时
	SocketTimeOut(clientfd, socketoption.recvtimeout, socketoption.sendtimeout, socketoption.lingertimeout);
	TRACE(_T("Connected\n"));
	while (!set_thread_exit_flag)
	{
		SocketRecv(clientfd, recvbuff, BUFLENGTH, rt);
		if (rt.nbytes > 0)
		{
			if ((recvbuff[0] == 'Q') && rt.nbytes == 1)//测试用
			{
				return_value = 0;
				break;
			}

			stickdismantle_locker->Lock();
			return_value = StickDismantleProtocol(clientfd, recvbuff, rt.nbytes, temp_option);
			stickdismantle_locker->Unlock();

			memset(recvbuff, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];

		}
		else if ((rt.nbytes == -1) && (rt.nresult==1))
		{
			TRACE(_T("SocketRecv Timeout\n"));
		}
		else if ((rt.nresult == -1))
		{
			TRACE(_T("Client close socket\n"));
			return_value = -1;
			break;
		}
		else
		{
		}

	}

	return return_value;
}
int JProtocol::StickDismantleProtocol(HSocket clientfd, char *ptr, int ptr_len, StickDismantleOptions_t &option)
{
	int return_value = 0;
	int32_t recv_length = ptr_len;
	std::string len_str;
	len_str.clear();

	if (!ptr || ptr_len>BUFLENGTH) return -1;
	memcpy(&option.data[option.count], ptr, ptr_len);

Start:

	if ((option.data[0] == PROTOCOL_HEAD) && (recv_length >= 5) && (option.bytes_remained == 0))//protocol start
	{
		memcpy((void*)len_str.c_str(), &option.data[1], PROTOCOL_PACKAGE_LENGTH_SIZE);
		sscanf(len_str.c_str(), "%D", &option.pro_length);//string->int

		option.bytes_remained = option.pro_length - (recv_length - 5);

		if (option.bytes_remained == 0)
		{
			TRACE(_T("recv_okay\n"));
			PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);
			//clear temp
			option.pro_length = 0;
			option.count = 0;
			memset(option.data, 0, BUFLENGTH);//clear data;
		}
		else if (option.bytes_remained > 0)//need  to stick the buff
		{
			TRACE(_T("need to stick the buff\n"));
			option.count += recv_length;

		}
		else//recv_length >(pro_length+5) (bytes_remained < 0)//need  to dismantle the buff
		{
			TRACE(_T("need  to dismantle the buff\n"));
			PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);

			memcpy(&option.data[0], &option.data[option.pro_length + 5], recv_length - 5 - option.pro_length);
			memset(&option.data[recv_length - option.pro_length - 5], 0x00, BUFLENGTH - (recv_length - option.pro_length - 5));


			recv_length = recv_length - 5 - option.pro_length;
			option.bytes_remained = 0;
			option.count = 0;
			goto Start;

		}

	}
	else if (option.bytes_remained >0)
	{
		if (option.bytes_remained == recv_length)
		{
			TRACE(_T("recv_okay\n"));
			PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);
			//clear temp
			option.pro_length = 0;
			option.count = 0;
			option.bytes_remained = 0;
			memset(option.data, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
		}
		else if (option.bytes_remained > recv_length)
		{
			TRACE(_T("need to stick the buff-2\n"));
			option.bytes_remained -= recv_length;
			option.count += recv_length;
			//continue;//wait the 
		}
		else//bytes_remained < recv_length
		{
			if ((option.data[option.pro_length + PROTOCOL_PACKAGE_LENGTH_SIZE] == '}'))
			{
				TRACE(_T("need  to dismantle the buff-2\n"));
				PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);

				memcpy(&option.data[0], &option.data[option.pro_length + 5], (option.count + recv_length - option.pro_length - 5));
				memset(&option.data[option.count + recv_length - option.pro_length - 5], 0x00, BUFLENGTH - (option.count + recv_length - option.pro_length - 5));

				recv_length = option.count + recv_length - option.pro_length - 5;
				option.bytes_remained = 0;
				option.count = 0;
				//goto Start;
			}
			else
			{
				TRACE(_T("packet loss and wait to stick buff-3\n"));
				memcpy(&option.data[0], &option.data[option.count], recv_length);
				memset(&option.data[recv_length], 0x00, BUFLENGTH - (recv_length));
				//recv_length = recv_length;
				option.bytes_remained = 0;
				option.count = 0;
			}
			goto Start;

		}

	}
	else//bytes_remained < 0
	{
		option.pro_length = 0;
		option.count = 0;
		option.bytes_remained = 0;
		memset(option.data, 0, BUFLENGTH);
		SocketClearRecvBuffer(clientfd);
		TRACE(_T("Recv err data and clear temp!!!\n"));
	}

	return return_value;

}
bool JProtocol::CloseSocket(HSocket sockfd)
{
	closesocket(sockfd);
	return true;

}

int JProtocol::PhySocketSendData(HSocket Objsoc, char *buff, int send_len)
{
	int count = 0;
	transresult_t rt; 
	rt.nbytes = 0;
	rt.nresult = 0;

	do
	{
		SocketSend(Objsoc, &buff[count], (send_len - count), rt);
		if ((rt.nbytes > 0))
		{
			TRACE(("send length is %d\n"), rt.nbytes);
			count += rt.nbytes;
		}
		else if ((rt.nbytes == -1) && (rt.nresult == 1))
		{
			TRACE(_T("SocketSend Timeout\n"));

		}
		else if ((rt.nbytes == -1) && (rt.nresult == -1))
		{
			TRACE(_T("Client close socket\n"));
			count = -1;
			return count;
		}


	} while ((send_len - count) != 0);

	count = 0;
	return count;


}
int JProtocol::SendDataToTheThirdParty(HSocket fd, std::string buff)
{
	int return_value = 0;
	int send_len = 0;
	HSocket Objsoc = 0;
	std::map<HSocket, struct sockaddr_in> ::iterator it;

	stringstream ss;
	ss<< buff.size();
	Objsoc = fd;

	phy_fragment_t phy_fragment;
	memset(phy_fragment.fragment_element, 0x00, sizeof(phy_fragment));

	phy_fragment.transport_protocol_fragment.head = PROTOCOL_HEAD;

	if (buff.size() < 1000)
	{
		phy_fragment.transport_protocol_fragment.payload_len[0] = '0';
		memcpy(&(phy_fragment.transport_protocol_fragment.payload_len[1]), ss.str().c_str(), PROTOCOL_PACKAGE_LENGTH_SIZE - 1);
		//sprintf_s如下操作即可
		//sprintf_s(&(phy_fragment.transport_protocol_fragment.payload_len[1]), 10,  "%d", buff.size());
	}
	else
	{
		memcpy(&(phy_fragment.transport_protocol_fragment.payload_len[0]), ss.str().c_str(), PROTOCOL_PACKAGE_LENGTH_SIZE);
		//sprintf_s(&(phy_fragment.transport_protocol_fragment.payload_len[0]), 10,  "%d", buff.size());

	}
	
	memcpy(phy_fragment.transport_protocol_fragment.json_payload, buff.c_str(), buff.size());
	send_len = strlen(phy_fragment.fragment_element);

	if (Objsoc != INVALID_SOCKET)
	{
		if (Objsoc == 0)
		{
			clientmap_locker->Lock();
			for (it = clientmap.begin(); it != clientmap.end(); ++it)
			{
				Objsoc = it->first;
				return_value = PhySocketSendData(Objsoc, phy_fragment.fragment_element, send_len);
			}
			clientmap_locker->Unlock();

		}
		else
		{
			return_value = PhySocketSendData(Objsoc, phy_fragment.fragment_element, send_len);
		}

	}
	else
	{
		TRACE(_T("socket is empty!!!\n"));
	}

	Objsoc = INVALID_SOCKET;
	return return_value;
}

void JProtocol::ConnectReply(HSocket dst_fd, std::string status, std::string reason)
{
	
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "Connect";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	//SendBuf.clear();
	//send_root.clear();
	//send_arrayObj.clear();
	//send_item.clear();
	TRACE(_T("Send ConnectReply\n"));

}
void JProtocol::ConfigReply(HSocket dst_fd, int channel1_value, int channel2_value)
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


	send_root["identifier"] = CreateGuid();
	send_root["type"] = "Reply";
	send_root["name"] = "Listening";
	send_root["param"] = send_arrayObj3;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);

	TRACE(_T("Send ConfigReply\n"));


}
void JProtocol::QueryReply(HSocket dst_fd, int channel1_value, int channel2_value)
{
	Json::Value send_root;
	Json::Value send_arrayObj1;
	Json::Value send_arrayObj2;
	Json::Value send_item1;
	Json::Value send_item2;
	Json::StyledWriter style_write;


	send_item2["channel1"] = channel1_value;
	send_item2["channel2"] = channel2_value;
	send_arrayObj2.append(send_item2);
	send_item1["listening"] = send_arrayObj2;
	if ((channel1_value != 0) && (channel2_value != 0))
	{
		send_item1["status"] = "success";
		send_item1["reason"] = "";
	}
	else
	{
		send_item1["status"] = "fail";
		if (channel1_value ==0 && channel2_value ==0)
			send_item1["reason"] = "channel1,2 is unset";
		else if (channel1_value == 0 && channel2_value != 0)
			send_item1["reason"] = "channel1 is unset";
		else
			send_item1["reason"] = "channel2 is unset";
	}

	send_arrayObj1.append(send_item1);

	send_root["identifier"] = CreateGuid();
	send_root["type"] = "Reply";
	send_root["name"] = "Query";

	send_root["param"] = send_arrayObj1;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	TRACE(_T("Send QueryReply\n"));

}
void JProtocol::CallRequestReply(HSocket dst_fd, std::string status, std::string reason)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "CallRequest";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	TRACE(_T("Send CallRequestReply\n"));

}
void JProtocol::CallReleaseReply(HSocket dst_fd, std::string status, std::string reason)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "CallRelease";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	TRACE(_T("Send CallReleaseReply\n"));

}
void JProtocol::CallStartNotify(HSocket dst_fd, int src, int dst, std::string channel)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["src"] = src;
	send_item["dst"] = dst;
	send_item["channel"] = channel;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Noitify";
	send_root["name"] = "CallStart";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);

	TRACE(_T("Send CallStartNotify\n"));

}
void JProtocol::CallEndNotify(HSocket dst_fd, int src, int dst, std::string channel)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["src"] = src;
	send_item["dst"] = dst;
	send_item["channel"] = channel;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Noitify";
	send_root["name"] = "CallEnd";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	TRACE(_T("Send CallEndNotify\n"));

}

std::string JProtocol::CreateGuid()
{
	std::string strGuid = "", strValue;
	srand((unsigned)clock()); /*播种子*/
	for (int i = 0; i < 32; i++)
	{
		float Num = (float)(rand() % 16);
		int nValue = (int)floor(Num);
		/*GOSPRINTF((char *)strValue.c_str(), 2, "%0x", nValue);*/
		sprintf((char *)strValue.c_str(),"%0x", nValue);
		strGuid.insert(i, strValue.c_str());
	}
	return strGuid;
	

}