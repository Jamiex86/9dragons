// XTimeCounter.cpp: implementation of the _XTimeCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include "XTimeCounter.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




_XTimeCounter::_XTimeCounter()
{
	
}

_XTimeCounter::~_XTimeCounter()
{
	
}

void _XTimeCounter::Init( void )
{
	m_Frequency					= 0;
	m_Resolution				= 0.0f;
	m_Performance_timer_start	= 0;
	m_Start						= 0.0f;
	m_Elapsed					= 0.0f;
	m_ElapsedMiliSecond			= 0.0f;
	
	//	memset(&timer, 0, sizeof(timer));						// Clear Our Timer Structure
	QueryPerformanceFrequency((LARGE_INTEGER *) &m_Frequency);
	QueryPerformanceCounter((LARGE_INTEGER *) &m_Performance_timer_start);
	//	performance_timer			= TRUE;						// Set Performance Timer To TRUE
	m_Resolution				= (float) (((double)1.0f)/((double)m_Frequency));
	//	performance_timer_elapsed	= performance_timer_start;
	m_Start = GetTime();
	_XLog("_XTimerCounter initialized");
}

float _XTimeCounter::GetTime()										// Get Time In Milliseconds
{
	__int64 time;	
	QueryPerformanceCounter((LARGE_INTEGER *) &time);
	return ( (float) ( time - m_Performance_timer_start) * m_Resolution);
}

void _XTimeCounter::Update()
{
	float curtime = GetTime();
	m_ElapsedMiliSecond	= (curtime - m_Start);
	m_Elapsed	  =	m_ElapsedMiliSecond * 1000.0f;
	m_Start		  = curtime;
}