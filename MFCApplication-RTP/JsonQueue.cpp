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
	WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
	m_list.clear();
	ReleaseMutex(m_hLocker);;//�ȴ�����������

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
	WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
	m_list.push_back(&fifobuff[fifo_counter][0]);//��һ�����ݰ���ַ��������β
	ReleaseMutex(m_hLocker);;//�ȴ�����������
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
			sBuffer = m_list.front();//���������һ�����ݰ���ַ��sbuffer

			WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
			m_list.pop_front();//��ɾ�������һ�����ݰ���ַ
			ReleaseMutex(m_hLocker);;//�ȴ�����������

			memcpy(packet, sBuffer, 512);
			len = 512;
	}
	else
	{
		return -1;

	}
	return 0;


}



