
//==============================================================================
//
//   cthread.h - exported_h
//   
//==============================================================================
//  Copyright (C) 2024 Luminator Tech. Group  <guillaume.plante@luminator.com>
//==============================================================================

#ifndef __CTHREAD_H__
#define __CTHREAD_H__

//project dependencies

//win32 specific
#include <windows.h>

//end project dependencies

class CThread {

public:
	//constructors
	CThread ();
	CThread(const char *id);
	//destructor
	virtual ~CThread ();

	bool CreateThread ();
	void SetThreadIdentifier(const char *id) { strncpy(threadIdentifier, id, MAX_PATH - 1); };
	int	IsCreated() {	return (this->hThread != NULL);	}

	bool IsRunning() { return isRunning; }

	DWORD		Timeout;

	HANDLE	GetThreadHandle ()
	{	return this->hThread;	}
	DWORD	GetThreadId ()
	{	return this->hThreadId;	}
	HANDLE	GetMainThreadHandle ()
	{	return this->hMainThread;	}
	DWORD	GetMainThreadId ()
	{	return this->hMainThreadId;	}

protected:

	//overrideable
	virtual unsigned long Process (void* parameter);	

	DWORD		hThreadId;
	HANDLE		hThread;
	DWORD		hMainThreadId;
	HANDLE		hMainThread;
	bool		isRunning;
	
private:

	static int runProcess (void* Param);
	static char threadIdentifier[MAX_PATH];
	
};

struct param {
	CThread*	pThread;
};

#endif // __CTHREAD_H__