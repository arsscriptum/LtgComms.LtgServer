
//==============================================================================
//
//   lock_guard.h
//
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================

#include <windows.h>
#include <memory.h>

#ifndef __LOCK_GUARD_H__
#define __LOCK_GUARD_H__


class LockGuard
{
	friend class CLockGuard;
public:
	LockGuard()
	{ 
		InitializeCriticalSection(&m_tgSection);  
	}

	virtual ~LockGuard()
	{
		DeleteCriticalSection(&m_tgSection);
	}
	void Lock()
	{
		EnterCriticalSection(&m_tgSection);
	}

	BOOL TryLock()
	{
		return TryEnterCriticalSection(&m_tgSection);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_tgSection);
	}

private:
	CRITICAL_SECTION	m_tgSection;
};

class CLockGuard
{
public:

	CLockGuard(LockGuard* cs)
	{
		m_cs = cs;
		
		if (m_cs != NULL)
		{
			m_cs->Lock();
		}
	}

	void Unlock()
	{
		if (m_cs)
		{
			m_cs->UnLock();
			m_cs = NULL;
		}
		
	}

	virtual ~CLockGuard(void)
	{
		Unlock();
	}

private:
	LockGuard* m_cs;
};


#endif //__LOCK_GUARD_H__