/*
 * myrtpsender.cpp
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 
#include "myrtpsender.h"


MyRTPSender::MyRTPSender()
{
	
}

MyRTPSender::~MyRTPSender()
{	

}

void MyRTPSender::Rtp_Sender_Init(uint16_t destport, uint32_t ssrc)
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

	sessparams.SetAcceptOwnPackets(false);
	sessparams.SetUsePredefinedSSRC(true);//set SSRC for rtp-send	
	sessparams.SetPredefinedSSRC(ssrc);
	
	status = Create(sessparams, &transparams);
	//fprintf(stderr,"status :%d\n", status);
	checkerror(status);
	
	RTPIPv4Address addr(localip, destport);

	status = AddDestination(addr);
	checkerror(status);
	
}


void MyRTPSender:: Set_ParamsForSender()
{
	
	this->SetDefaultPayloadType(96);//设置传输类型  
    this->SetDefaultMark(false);      //设置位  
    this->SetTimestampUnit(1.0/8000.0); //设置采样间隔  
    this->SetDefaultTimestampIncrement(160);//设置时间戳增加间隔  
	
	
}
void MyRTPSender::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "MyRTPSender_ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		while (1);
		//exit(-1);
	}
}





