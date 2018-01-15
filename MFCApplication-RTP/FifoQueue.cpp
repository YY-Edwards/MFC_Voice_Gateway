/*
 * FifoQueue.c
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 
#include "stdafx.h"
#include "FifoQueue.h"

FifoQueue::FifoQueue()
{
	
	m_hSemaphore = CreateSemaphore(NULL, 0, 100, NULL);
	int err = 0 ;
	for (int i = 0; i < 20; i++){
		memset(&fifobuff[i][0], 0x00, 512);
	}
	fifo_counter = 0;
	InitializeCriticalSection(&cs);

	
}
FifoQueue::~FifoQueue()
{
	CloseHandle(m_hSemaphore);
	DeleteCriticalSection(&cs);
	
}

void FifoQueue::ClearQueue()
{
	EnterCriticalSection(&cs);
	m_list.clear();
	LeaveCriticalSection(&cs);

}


bool  FifoQueue::QueueIsEmpty()
{
	return(m_list.empty());
}
bool FifoQueue::PushToQueue(void *packet, int len)
{
	int err = 0;
	static int counter = 0;

	EnterCriticalSection(&cs);

	memcpy_s(&fifobuff[fifo_counter][0], len, packet, len);
	m_list.push_back(&fifobuff[fifo_counter][0]);//将一个数据包地址插入链表尾
	fifo_counter++;
	if (fifo_counter > 19){
		fifo_counter = 0;
	}

	LeaveCriticalSection(&cs);

	ReleaseSemaphore(m_hSemaphore, 1, NULL);//触发信号量
	return true;

}

int32_t FifoQueue::TakeFromQueue(void *packet, int& len, int waittime)
{

	char* sBuffer = NULL;

	DWORD ret = WaitForSingleObject(m_hSemaphore, waittime);//等待信号量触发
	if (ret != WAIT_OBJECT_0)
	{
		return ret;
	}

	if (!m_list.empty())
	{
			EnterCriticalSection(&cs);

			sBuffer = m_list.front();//返回链表第一个数据包地址给sbuffer
			m_list.pop_front();//并删除链表第一个数据包地址

			LeaveCriticalSection(&cs);
			memcpy_s(packet, 512, sBuffer, 512);
			len = 512;
			return ret;
	}
	else
	{
		return -1;

	}


}



