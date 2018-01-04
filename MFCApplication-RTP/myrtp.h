/*
 * myrtp.h
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 
#ifndef myrtp_h_
#define myrtp_h_ 

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdbool.h>
#include <stdint.h>
#include <process.h>

using namespace jrtplib;
using namespace std;

class MyRTP :public RTPSession
{

public:
		
		MyRTP();
		~MyRTP();

		void Rtp_Init(uint16_t portbase, uint16_t destport, uint32_t ssrc);
		void Set_ParamsForSender();
		void SendRTPPayloadData(uint8_t* buff, uint32_t buff_length);
		uint8_t *GetRTPPayloadData();
		uint32_t GetRTPPayloadDataLength();
		uint32_t GetRTPSSRC();
		void Set_Exit_Flag()   {Thread_Exit_Flag = true; };
protected:

		static int OnPollThread(void* p);
		void OnPollThreadFunc();
		void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);	
	
	
	private:
		void checkerror(int rtperr);
		bool Poll_Thread_IsActive;
		bool Thread_Exit_Flag;
		uint32_t ssrc;
		uint32_t payloaddatalength;
		uint8_t* payloaddata;
		HANDLE Rx_RTP_Handle;
		
};








#endif