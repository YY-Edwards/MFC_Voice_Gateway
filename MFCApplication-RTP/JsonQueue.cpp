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

	/*sBuffer = new char[512];
	memset(sBuffer, 0x00, 512);*/

	
}
JsonQueue::~JsonQueue()
{
	CloseHandle(m_hLocker);
	CloseHandle(m_hSemaphore);
	//if (sBuffer != NULL)
	//{
	//	delete[]sBuffer;
	//	sBuffer = NULL;
	//}
	
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

	memcpy_s(&fifobuff[fifo_counter][0], 512,  packet, len);
	WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
	m_list.push_back(&fifobuff[fifo_counter][0]);//��һ�����ݰ���ַ��������β
	ReleaseMutex(m_hLocker);;//�ȴ�����������
	fifo_counter++;
	if (fifo_counter > 19){
		fifo_counter = 0;
	}
	ReleaseSemaphore(m_hSemaphore, 1, NULL);//�����ź���
	return true;

}

int32_t JsonQueue::TakeFromQueue(void *packet, int& len)
{

	 char* sBuffer;

	DWORD ret = WaitForSingleObject(m_hSemaphore, 200);//�ȴ��ź�������
	if (ret != WAIT_OBJECT_0)
	{
		return ret;
	}

	//if (!m_list.empty())
	{
			sBuffer = m_list.front();//���������һ�����ݰ���ַ��sbuffer

			WaitForSingleObject(m_hLocker, INFINITE);//�ȴ�����������
			m_list.pop_front();//��ɾ�������һ�����ݰ���ַ
			ReleaseMutex(m_hLocker);//�ȴ�����������

			memcpy_s(packet, 512, sBuffer, 512);
			len = 512;
			return 0;
	}
	/*else
	{
		return -1;

	}*/


}



