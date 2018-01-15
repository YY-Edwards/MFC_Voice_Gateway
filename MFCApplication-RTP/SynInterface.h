/*
* SynInterface.h
*
* Created: 2017/12/19
* Author: EDWARDS
*/
#ifndef _SYNINTERFACE_H
#define _SYNINTERFACE_H

#include "Common.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <semaphore.h> 
#endif

//锁接口类  
class ILock
{
public:
	virtual ~ILock() {}

	virtual void Lock() const = 0;
	virtual void Unlock() const = 0;
};

//信号量接口类 
class ISem
{
public:
	virtual ~ISem() {}

	virtual int SemWait(int waittime) const = 0;
	virtual void SemPost() const = 0;
};

//事件/条件信号量接口类 
class ICond
{
public:
	virtual ~ICond() {}

	virtual int CondWait(int waittime) const = 0;
	virtual void CondTrigger(bool isbroadcast) const = 0;
};





//临界区锁类:速度快，消耗资源小 
class CriSection : public ILock
{
public:
	CriSection();
	~CriSection();

	virtual void Lock() const;
	virtual void Unlock() const;

private:
	GOCRITICAL_SECTION m_critclSection;
};

//互斥锁类：可以实现不同进程中线程之间互斥
class Mutex : public ILock
{
public:
	Mutex(LPCTSTR lockUniName);
	~Mutex();

	virtual void Lock() const;
	virtual void Unlock() const;

	GOMUTEX_T GetMutex(){ return m_mutex; }

private:
	GOMUTEX_T m_mutex;
};

//同步信号量
class MySynSem : public ISem
{
public:
	MySynSem();
	~MySynSem();

	virtual int SemWait(int waittime) const = 0;
	virtual void SemPost() const = 0;

private:
	GOSEM_T m_sem;

};

//条件信号量/事件
class MySynCond : public ICond
{
public:
	MySynCond ();
	~MySynCond ();

	virtual int CondWait(int waittime) const = 0;
	virtual void CondTrigger(bool isbroadcast) const = 0;


private:
	Mutex *m_condlock;
	GOCOND_T m_cond;
	 

};



#endif