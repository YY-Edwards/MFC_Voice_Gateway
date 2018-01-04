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
	poll_thread_isactive =false;
	rx_rtp_handle = NULL;
	payloaddata =NULL;
	payloaddatalength =0;
	ssrc =0;
	thread_exit_flag = false;
}

MyRTP::~MyRTP()
{	
	
}

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
		
		RTPTime::Wait(RTPTime(0,20));//20ms
		//num++;
		
		if (thread_exit_flag)break;
		//if(num>230)break;
		
	}
	
	BYEDestroy(RTPTime(5, 0), 0, 0);
	//TRACE(_T("exit poll thread \r\n"));
	std::cout << "exit poll thread "<<std::endl;
	
	//return 0;
	
}

void MyRTP::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	// You can inspect the packet and the source's info here
	std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
	TRACE(_T("Got packet with extended sequence number:%d ", rtppack.GetExtendedSequenceNumber()));
	TRACE(_T("from SSRC %d \r\n", srcdat.GetSSRC()));
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

	//poll_thread_isactive = true;
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





