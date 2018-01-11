/*
 * FifoQueue.h
 *
 * Created: 2018/01/03
 * Author: EDWARDS
 */ 

#ifndef FifoQueue_h_
#define FifoQueue_h_
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <queue>
#include <stdint.h>
#include <process.h>
#include "Common.h"

class FifoQueue 
{
	
	public:
		FifoQueue();
		~FifoQueue();

	public:
		bool  			PushToQueue(void *packet, int len);
		int32_t 		TakeFromQueue(void *packet, int& len, int waittime =200);
		void			ClearQueue();
		bool 			QueueIsEmpty();

	private:
		//init mutex
		HANDLE m_hLocker;
		HANDLE m_hSemaphore;
		std::list<char *>  	m_list;
		std::queue<char *>  m_queue;
		char fifobuff[20][512];
		volatile  uint32_t fifo_counter;
		//char* sBuffer;
		CRITICAL_SECTION cs;
	
};



#endif