/*
 * myrtpreceiver.cpp
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 
#include "myrtpreceiver.h"



MyRTPReceiver::MyRTPReceiver()
{
	//Poll_Thread_IsActive =false;
	Rx_RTP_Handle = NULL;
}

MyRTPReceiver::~MyRTPReceiver()
{	
	
}

int MyRTPReceiver::OnPollThread(void* p)
{
	MyRTPReceiver *arg = (MyRTPReceiver*)p;
	if (arg != NULL)
	{
		arg->OnPollThreadFunc();
	}
	return 0;
}

void MyRTPReceiver::OnPollThreadFunc()
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
		checkerror(status);
		
		RTPTime::Wait(RTPTime(0,20));//20ms
		num++;
		
		if(num>230)break;
		
	}
	
	BYEDestroy(RTPTime(5, 0), 0, 0);
	std::cout << "exit poll thread "<<std::endl;
	
	//return 0;
	
}

void MyRTPReceiver::ProcessRTPPacket(const RTPSourceData &srcdat,const RTPPacket &rtppack)
{
	// You can inspect the packet and the source's info here
	std::cout << "Got packet " << rtppack.GetExtendedSequenceNumber() << " from SSRC " << srcdat.GetSSRC() << std::endl;
}

void MyRTPReceiver::Rtp_Receiver_Init(uint16_t portbase)
{
	
	int status = 0;
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
	transparams.SetPortbase(portbase);
	
	status = Create(sessparams, &transparams);
	//fprintf(stderr,"status :%d\n", status);
	checkerror(status);

	//Poll_Thread_IsActive = true;
	Rx_RTP_Handle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void *))OnPollThread, this, 0, NULL);
	if (Rx_RTP_Handle == NULL)
	{
		std::cout<<"create thread failed"<<std::endl;
		system("pause");
		//return 0;
	}

	
}


void MyRTPReceiver::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "MyRTPReceiver_ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		while (1);
		//exit(-1);
	}
}





