/*
 * myrtp.cpp
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 

#pragma once

#include "stdafx.h"
#include "myrtp.h"

MyRTP::MyRTP()
{
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);//init socket

	poll_thread_isactive =false;
	rx_rtp_handle = NULL;
	payloaddata =NULL;
	payloaddatalength =0;
	ssrc =0;
	set_thread_exit_flag = false;
	thread_exited_flag = false;
}

MyRTP::~MyRTP()
{	
	SetThreadExitFlag();
	do
	{
		Sleep(30);
	} while (!(IsThreadHasExit()));
	//if (channel1RTP->IsActive())channel1RTP->BYEDestroy(RTPTime(0, 10), "Time's up", 9);

	CloseHandle(rx_rtp_handle);

	WSACleanup();
	TRACE(_T("Destory: MyRTP \n"));

}

//UINT MyRTP::OnPollThread(LPVOID p)
//{
//	MyRTP *arg = (MyRTP*)p;
//	if (arg != NULL)
//	{
//		arg->OnPollThreadFunc();
//	}
//	return 0;
//}
int MyRTP::OnPollThread(void* p)
{
	MyRTP *arg = (MyRTP*)p;
	if (arg != NULL)
	{
		arg->OnPollThreadFunc();
	}
	return 0;
}

void MyRTP::OnPollThreadFunc()
{
	int status = 0;
	uint32_t num =0;
	poll_thread_isactive = true;

	RTPTime delay(0.020);
	for(;;){
		
		BeginDataAccess();
			
		// check incoming packets
		if (GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;
				RTPSourceData *srcdat;
				
				srcdat = GetCurrentSourceInfo();
				
				while ((pack = GetNextPacket()) != NULL)
				{
					ProcessRTPPacket(*srcdat,*pack);
					DeletePacket(pack);
				}
			} while (GotoNextSourceWithData());
		}
			
		EndDataAccess();
		status = Poll();
		CheckError(status);

		//RTPTime::Wait(RTPTime(0,20));//20ms
		RTPTime::Wait(delay);

		if (set_thread_exit_flag)break;
		
	}
	
	BYEDestroy(RTPTime(5, 0), 0, 0);
	TRACE(("exit poll thread: 0x%x\r\n"), GetCurrentThreadId());
	//std::cout << "exit poll thread "<<std::endl;

	thread_exited_flag = true;
	//return 0;
	
}

void MyRTP::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	// You can inspect the packet and the source's info here
	std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
	TRACE(_T("Got packet with extended sequence number:%d "), rtppack.GetExtendedSequenceNumber());
	TRACE(_T("from SSRC %d \r\n"), srcdat.GetSSRC());
	payloaddatalength = rtppack.GetPayloadLength();
	ssrc = srcdat.GetSSRC();
	payloaddata = rtppack.GetPayloadData();

}

void MyRTP::Rtp_Init(uint16_t portbase, uint16_t destport, uint32_t ssrc)
{
	
	int status = 0;
	uint8_t localip[]={127,0,0,1};
	// Now, we'll create a RTP session, set the destination, send some
	// packets and poll for incoming data.

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	// IMPORTANT: The local timestamp unit MUST be set, otherwise
	//            RTCP Sender Report info will be calculated wrong
	// In this case, we'll be sending 10 samples each second, so we'll
	// put the timestamp unit to (1.0/10.0)
	sessparams.SetOwnTimestampUnit(1.0 / 8000.0);

	sessparams.SetAcceptOwnPackets(true);
	sessparams.SetUsePredefinedSSRC(true);//set SSRC for rtp-send	
	sessparams.SetPredefinedSSRC(ssrc);
	
	transparams.SetPortbase(portbase);
	status = Create(sessparams, &transparams);
	//fprintf(stderr,"status :%d\n", status);
	CheckError(status);
	
	RTPIPv4Address addr(localip, destport);

	status = AddDestination(addr);
	CheckError(status);

	//CWinThread* MyThread = AfxBeginThread(OnPollThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	rx_rtp_handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void *))OnPollThread, this, 0, NULL);
	if (rx_rtp_handle == NULL)
	{
		std::cout<<"create thread failed"<<std::endl;
		system("pause");
	}

	
}


void MyRTP::Set_ParamsForSender()
{
	
	SetDefaultPayloadType(96);//设置传输类型  
    SetDefaultMark(false);      //设置位  
    SetTimestampUnit(1.0/8000.0); //设置采样间隔  
    SetDefaultTimestampIncrement(160);//设置时间戳增加间隔  
	
	
}

void MyRTP::SendRTPPayloadData(uint8_t* buff, uint32_t buff_length)
{
	int status = this->SendPacket(buff, buff_length, 0, false, 160);
	CheckError(status);
	
}

uint8_t *MyRTP::GetRTPPayloadData()
{
	
	return payloaddata;
}

uint32_t  MyRTP::GetRTPPayloadDataLength()
{
	return payloaddatalength;
	
}

uint32_t  MyRTP::GetRTPSSRC()
{
	return ssrc;
	
}


void MyRTP::CheckError(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "MyRTPReceiver_ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		while (1);
		//exit(-1);
	}
}





