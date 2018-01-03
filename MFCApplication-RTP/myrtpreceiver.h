/*
 * myrtpreceiver.h
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 
#ifndef myrtpreceiver_h_
#define myrtpreceiver_h_ 


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

class MyRTPReceiver :public RTPSession
{

public:
		
		MyRTPReceiver();
		~MyRTPReceiver();

		void Rtp_Receiver_Init(uint16_t portbase);//defaul port:8020
		
protected:
	
	static int OnPollThread(void* p);
	void OnPollThreadFunc();
	void ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack);	
	
	
	private:
		bool Poll_Thread_IsActive;
		HANDLE Rx_RTP_Handle;
		void checkerror(int rtperr);

		
};


#endif

