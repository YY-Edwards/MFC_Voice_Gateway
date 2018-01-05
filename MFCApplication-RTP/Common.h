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


#pragma pack(1)
struct ResponeData
{
	std::string identifier;
	std::string type;
	std::string name;
	std::string param;
	uint32_t src_id;
	uint32_t dst_id;
	uint16_t channel;
	uint16_t status;

};
#pragma pack()




#endif