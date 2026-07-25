#include "stdafx.h"
#include "XTimer.h"
#include <windows.h>
#include <mmsystem.h>

////////////////////////////////////////////
// 생성자 
_XTimer::_XTimer()
{
	Clear();
}


////////////////////////////////////////////
// 소멸자 
_XTimer::~_XTimer()
{
}




////////////////////////////////////////////
// init
void _XTimer::Init(int tm)
{
	// 현재의 시간을 구한다.
	this->m_thistime[tm]	= ::timeGetTime() * 0.001f;
	this->m_oldtime[tm]		= this->m_thistime[tm];
}
void _XTimer::Init_All()
{
	// 현재의 시간을 구한다.
	float current_time( ::timeGetTime() * 0.001f );

	int count(MAX_TIMER_COUNT);
	while( count-- )
	{
		this->m_thistime[count]	= current_time;
		this->m_oldtime[count]	= current_time;
	}
}


////////////////////////////////////////////
// uninit
void _XTimer::UnInit(int tm)
{
	// 현재의 시간을 구한다.
	this->m_oldtime[tm]	= -999.0f;
}
void _XTimer::UnInit_All()
{
	// 현재의 시간을 구한다.
	int count(MAX_TIMER_COUNT);
	while( count-- )
	{
		this->m_oldtime[count]	= -999.0f;
	}
}


////////////////////////////////////////////
// 프레임 스킵을 한다. 
// (1/1000초(0.001초)까지 계산 가능)
// tm    : time ID
// frame : 지연 시간
BOOL _XTimer::FrameSkip(int tm, float frame)
{
	// 현재의 시간을 구한다.
	this->m_thistime[tm]	= ::timeGetTime() * 0.001f;

	// 현재 프레임을 더한다.
	this->m_thisframe[tm]++;
		
	if( (this->m_thistime[tm] - this->m_oldtime[tm]) > frame) // frame수 만큼 시간이 흘렀는지 검사한다.
    {
		// 현재의 시간을 저장한다.
		this->m_oldtime[tm]		= this->m_thistime[tm];     

		// FPS
		this->m_fps[tm]	= this->m_thisframe[tm];
		// 프레임을 초기화 한다.
		this->m_thisframe[tm] = 0;

		return TRUE;
	}


	return FALSE;
}










void _XTimer::Clear()
{
	int index = MAX_TIMER_COUNT;

	while( index-- )
	{
		// frame time
		this->m_thistime[index]		= 0.0f;
		this->m_oldtime[index]		= 0.0f;

		this->m_thisframe[index]	= 0;

		this->m_fps[index]			= 0;

	}
}
