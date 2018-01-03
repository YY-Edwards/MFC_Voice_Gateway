/*
 * myrtpsender.h
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 
#ifndef myrtpsender_h_
#define myrtpsender_h_ 

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

using namespace jrtplib;
using namespace std;

class MyRTPSender :public RTPSession
{

public:
		
		MyRTPSender();
		~MyRTPSender();

		void Rtp_Sender_Init(uint16_t destport, uint32_t ssrc);
		void Set_ParamsForSender();
		
protected:
	
	private:
		void checkerror(int rtperr);

		
};


#endif

