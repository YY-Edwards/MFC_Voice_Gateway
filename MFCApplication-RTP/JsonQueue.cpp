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
	m_hSemaphore = CreateSemaphore(NULL, 0, 100, NULL);
	int err = 0 ;
	for (int i = 0; i < 20; i++){
		memset(&fifobuff[i][0], 0x00, 512);
	}
	fifo_counter = 0;
	InitializeCriticalSection(&cs);

	
}
JsonQueue::~JsonQueue()
{
	CloseHandle(m_hLocker);
	CloseHandle(m_hSemaphore);
	DeleteCriticalSection(&cs);
	
}

void JsonQueue::ClearQueue()
{
	EnterCriticalSection(&cs);
	//WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
	m_list.clear();
	//ReleaseMutex(m_hLocker);;//�ȴ�����������
	LeaveCriticalSection(&cs);

}


bool  JsonQueue::QueueIsEmpty()
{
	return(m_list.empty());
	//return(m_queue.empty());
}
bool JsonQueue::PushToQueue(void *packet, int len)
{
	int err = 0;
	static int counter = 0;

	memcpy_s(&fifobuff[fifo_counter][0], len, packet, len);
	//WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
	EnterCriticalSection(&cs);
	//m_queue.push(&fifobuff[fifo_counter][0]);
	m_list.push_back(&fifobuff[fifo_counter][0]);//��һ�����ݰ���ַ��������β
	fifo_counter++;
	if (fifo_counter > 19){
		fifo_counter = 0;
	}
	LeaveCriticalSection(&cs);
	//m_list.push_back(&fifobuff[fifo_counter][0]);//��һ�����ݰ���ַ��������β
	//ReleaseMutex(m_hLocker);;//�ȴ�����������
	//fifo_counter++;
	//if (fifo_counter > 19){
	//	fifo_counter = 0;
	//}
	ReleaseSemaphore(m_hSemaphore, 1, NULL);//�����ź���
	return true;

}

int32_t JsonQueue::TakeFromQueue(void *packet, int& len)
{

	char* sBuffer = NULL;

	DWORD ret = WaitForSingleObject(m_hSemaphore, 200);//�ȴ��ź�������
	if (ret != WAIT_OBJECT_0)
	{
		return ret;
	}

	if (!m_list.empty())
	//if (!m_queue.empty())
	{
			//sBuffer = m_list.front();//���������һ�����ݰ���ַ��sbuffer
			EnterCriticalSection(&cs);
			sBuffer = m_list.front();//���������һ�����ݰ���ַ��sbuffer
			//sBuffer = m_queue.front();
			//WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
			m_list.pop_front();//��ɾ�������һ�����ݰ���ַ
			//m_queue.pop();
			//ReleaseMutex(m_hLocker);//�ȴ�����������
			LeaveCriticalSection(&cs);
			//if (sBuffer > (&fifobuff[19][0]))return -1;
			memcpy_s(packet, 512, sBuffer, 512);
			len = 512;
			return 0;
	}
	else
	{
		return -1;

	}


}



