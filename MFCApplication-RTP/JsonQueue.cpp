/*
 * JsonQueue.c
 *
 * Created: 2016/12/19
 * Author: EDWARDS
 */ 
#include "stdafx.h"
#include "JsonQueue.h"

JsonQueue::JsonQueue()
{
	
	m_hLocker = CreateMutex(nullptr, FALSE, (LPCWSTR)"queuelocker");
	int err = 0 ;
	for (int i = 0; i < 40; i++){
		memset(&fifobuff[i][0], 0x00, 512);
	}
	fifo_counter = 0;

	
}
JsonQueue::~JsonQueue()
{
	CloseHandle(m_hLocker);
	
}

void JsonQueue::ClearQueue()
{
	WaitForSingleObject(m_hLocker, INFINITE);//等待互斥句柄触发
	m_list.clear();
	ReleaseMutex(m_hLocker);;//等待互斥句柄触发

}


bool  JsonQueue::QueueIsEmpty()
{
	return(m_list.empty());
}
bool JsonQueue::PushToQueue(void *packet, int len)
{
	int err = 0;
	static int counter = 0;

	memcpy(&fifobuff[fifo_counter][0], packet, len);
	WaitForSingleObject(m_hLocker, INFINITE);//等待互斥句柄触发
	m_list.push_back(&fifobuff[fifo_counter][0]);//将一个数据包地址插入链表尾
	ReleaseMutex(m_hLocker);;//等待互斥句柄触发
	fifo_counter++;
	if (fifo_counter > 40){
		fifo_counter = 0;
	}
	return true;

}

uint32_t JsonQueue::TakeFromQueue(void *packet, int& len)
{

	char* sBuffer;

	if (!m_list.empty())
	{
			sBuffer = m_list.front();//返回链表第一个数据包地址给sbuffer

			WaitForSingleObject(m_hLocker, INFINITE);//等待互斥句柄触发
			m_list.pop_front();//并删除链表第一个数据包地址
			ReleaseMutex(m_hLocker);;//等待互斥句柄触发

			memcpy(packet, sBuffer, 512);
			len = 512;
	}
	else
	{
		return -1;

	}
	return 0;


}



