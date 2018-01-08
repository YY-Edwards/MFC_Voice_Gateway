#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdbool.h>
#include <stdint.h>
using namespace std;


#define						CHANNEL1RTPBASEPORT						55500
#define						CHANNEL1RTPDESTPORT						57400
#define						CHANNEL1RTPSSRC							1973

#define						CHANNEL2RTPBASEPORT						55600
#define						CHANNEL2RTPDESTPORT						57500
#define						CHANNEL2RTPSSRC							2012

#define						TCPPORT									57000
#define						BUFLENGTH								1024

#define						PROTOCOL_HEAD							0x01
#define						PROTOCOL_LENGTH							4

#pragma pack(1)
struct ResponeData
{
	std::string identifier;
	//std::string type;
	//std::string name;
	std::string key;

	std::string channel_id;
	int16_t channel1_id;
	int16_t channel2_id;

	int32_t src_id;
	int32_t dst_id;
	
	std::string  status;
	std::string  reason;

};
#pragma pack()


//PROTOCOL State Master.
typedef enum {
	PROTOCOL_UNCONNECTEDWAITINGSTATUS,
	PROTOCOL_UNCONNECTEDWAITINGSETLISTENING,
	PROTOCOL_CONNECTED
} MASTER_States;


//PROTOCOL Type Master.
typedef enum {
	REQUEST,
	REPLY,
	NOTIFY
} PROTOCOL_Types;


//PROTOCOL name Master.
typedef enum {
	CONNECT,
	LISTENING,
	QUERY,
	CALLREQUEST,
	CALLRELEASE,
	CALLSTART,
	CALLEND

} PROTOCOL_Names;


//param->listening->Listening_channles

typedef struct{

	uint32_t channel1_value;
	uint32_t channel2_value;

}Listening_Channels;


#pragma pack(1)
typedef struct{
	std::string status;
	std::string reason;
	uint32_t channel_value;

}Channel_Params;
#pragma pack()

//PROTOCOL Params(All.)
#pragma pack(1)
typedef struct{

	std::string key;
	std::string status;
	std::string reason;
	uint32_t src;
	uint32_t dst;
	std::string channel; 
	Listening_Channels Listening_channles;
	Channel_Params Channel1_Param;
	Channel_Params Channel2_Param;

}PROTOCOL_Params;
#pragma pack()

#pragma pack(1)
typedef struct{

	PROTOCOL_Names MASTER_State;
	//MASTER_States MASTER_State;
	std::string identifier;
	std::string type;
	std::string name;
	//PROTOCOL_Types PROTOCOL_Type;
	//PROTOCOL_Names PROTOCOL_Name;
	PROTOCOL_Params PROTOCOL_params;
		 
}PROTOCOL_Ctrlr;
#pragma pack()

/**
#pragma pack(1)
typedef struct{

	MASTER_States MASTER_State;
	char identifier[20];
	char type[20];
	char name[20];

	char key[128];
	char status[10];
	char reason[128];
	uint32_t src;
	uint32_t dst;
	char channel[10];

	Listening_Channels Listening_channles;

	char channel1_param_status[10];
	char channel1_param_reason[128];
	uint32_t channel1_param_value;

	char channel2_param_status[10];
	char channel2_param_reason[128];
	uint32_t channel2_param_value;

}DATA_Ctrlr;
#pragma pack()
**/


#endif