// XProcess.cpp: implementation of the XProcess class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "XProcess.h"
#include "X3DApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XProcessManager::_XProcessManager()
{	

	m_CurrProcess			= NULL; 
	m_PreviousProcess		= NULL;
	m_NProcesses			= 0; 
	m_bForceChangeProcess	= 0;

	InitializeCriticalSection(&m_cs);
	memset( m_ProcessStack, 0, sizeof(_XProcess*) * _MAXACTIVEPROCESSES );
}

_XProcessManager::~_XProcessManager()
{
	m_CurrProcess			= NULL; 
	m_PreviousProcess		= NULL;
	m_NProcesses			= 0; 
	m_bForceChangeProcess	= 0;
	
	memset( m_ProcessStack, 0, sizeof(_XProcess*) * _MAXACTIVEPROCESSES );	
	DeleteCriticalSection( &m_cs );
}

void _XProcessManager::Lock()
{
	//EnterCriticalSection(&m_cs);
}

void _XProcessManager::Unlock()
{
	//LeaveCriticalSection(&m_cs);
}

void _XProcessManager::ChangeProcess(_XProcess* process)
{
	//Lock();
	m_PreviousProcess = m_CurrProcess;

	// call finish loop of the current process
	if (m_PreviousProcess!=NULL )
	{		
		if (m_PreviousProcess->m_dwFlag & _XGPF_INITED)
			m_PreviousProcess->DestroyProcess();			
		m_PreviousProcess->m_dwFlag &= ~_XGPF_INITED;
	}

	// change current process 
	m_CurrProcess = process;
	if (process!=NULL) 
	{
		process->m_StackPlace=-1;		
		_XDWINPRINT( "ChangeProcess : ");
		if (m_PreviousProcess) 
		{
			_XDWINPRINT("			%s ", m_PreviousProcess->m_processname ? m_PreviousProcess->m_processname : "null");
		}
		else 
		{
			_XDWINPRINT ("NULL");
		}
		_XDWINPRINT ("			--> %s", m_CurrProcess->m_processname ? m_CurrProcess->m_processname : "null");		

		if( strlen(m_CurrProcess->m_processname) > 0 )
		{
			_XLog("========================================================================" );		       
			_XLog( "Change process : %s", m_CurrProcess->m_processname );
			_XLog("========================================================================" );		       
		}
		else
		{
			_XLog("========================================================================" );
			_XLog( "Change process : Unknown" );
			_XLog("========================================================================" );
		}
	}
	//Unlock();
}

void _XProcessManager::ForceChangeProcess(_XProcess* process)
{
	//Lock();
	m_bForceChangeProcess = 1;
	m_PreviousProcess     = m_CurrProcess;
	if( m_PreviousProcess!=NULL ) 
	{
		//assert (CurrProcess->dwFlag & GPF_INITED);
		if (m_CurrProcess->m_dwFlag & _XGPF_INITED)
			m_CurrProcess->DestroyProcess();
		
		m_CurrProcess->m_dwFlag &= ~_XGPF_INITED;
	}

	m_CurrProcess=process;
	if (process!=NULL) 
	{
		process->m_StackPlace=-1;		
		_XDWINPRINT ("ChangeProcess : ");
		if (m_PreviousProcess) {
			_XDWINPRINT ("			%s ", m_PreviousProcess->m_processname ? m_PreviousProcess->m_processname : "null");
		}
		else {
			_XDWINPRINT ("NULL");
		}
		_XDWINPRINT ("			--> %s", m_CurrProcess->m_processname ? m_CurrProcess->m_processname : "null");
	}
	//Unlock();
}

bool _XProcessManager::ProcessHandler(void)
{
	_XProcess *x = 0;

	if (m_CurrProcess==NULL) 
	{ 		
		return true;
	}

	//Lock();
	/*
	 * call init process of the current process if the process is changed or it is forced.
	 * it is called also if flag is cleared.
	 */
	if (m_PreviousProcess != m_CurrProcess || m_bForceChangeProcess || m_CurrProcess->m_dwFlag == 0) 
	{
		m_PreviousProcess = m_CurrProcess;
		m_bForceChangeProcess = 0;
		x = m_CurrProcess;
		x->m_dwFlag |= _XGPF_INITED;
		//Unlock();

		if( !x->InitializeProcess() )
		{
			return false;
		}
	}
	else {
		x = m_CurrProcess;
		//Unlock();
	}

	if (x && x->m_Initialized ) 
	{
		//FadeHandler(); LATER -> Fade In/Out Ã³¸®
		return x->Process();
	}

	return true;
}

void _XProcessManager::SuspendProcess(_XProcess* process)
{
	_XProcess *t1, *t2;

	// stack current process
	
	//Lock();
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->Draw();

	m_PreviousProcess=m_CurrProcess;
	m_ProcessStack[m_NProcesses]=m_PreviousProcess;
	if(m_PreviousProcess) m_PreviousProcess->m_StackPlace=m_NProcesses;
	m_NProcesses++;

	// if there exists PauseLoop, call it.
	if (m_PreviousProcess!=NULL) 
	{
		t1 = m_CurrProcess;
		t2 = m_PreviousProcess;
		m_CurrProcess->PauseProcess();
		m_CurrProcess->m_dwFlag |= _XGPF_PAUSED;
	}	
	
	_XDWINPRINT ("SuspendProcess : ");
	if (m_PreviousProcess) {
		_XDWINPRINT ("%s ", m_PreviousProcess->m_processname ? m_PreviousProcess->m_processname : "null");
	}
	else {
		_XDWINPRINT("NULL");
	}
	m_CurrProcess=process;
	_XDWINPRINT(" --> %s", m_CurrProcess->m_processname ? m_CurrProcess->m_processname : "null");
	//Unlock();	
}

void _XProcessManager::ResumeProcess(_XProcess* process)
{
	//Lock();
	// call current process's finish loop if exists
	m_PreviousProcess = m_CurrProcess;
	if (process != m_PreviousProcess) 
	{
		if (m_PreviousProcess!=NULL)
		{
			if (m_CurrProcess->m_dwFlag & _XGPF_INITED)
				m_CurrProcess->DestroyProcess();

			m_CurrProcess->m_dwFlag &= ~_XGPF_INITED;
			_XDWINPRINT ("ResumeProcess: call %s's FinishLoop", m_CurrProcess->m_processname);
		}
	}

	m_PreviousProcess = process;
	// popup the last process
	if (m_PreviousProcess == NULL) 
	{
		if (m_NProcesses > 0) 
		{
			m_PreviousProcess = m_ProcessStack[m_NProcesses-1];	
			_XDWINPRINT ("ResumeProcess: poping up %s", m_PreviousProcess->m_processname);
		}
	}

	// call unpauseloop of the stacked process if exists
	if (m_PreviousProcess != NULL) 
	{
		if ((m_ProcessStack[m_PreviousProcess->m_StackPlace] >= 0) &&
			(m_ProcessStack[m_PreviousProcess->m_StackPlace] == m_PreviousProcess)) 
		{
			//assert (PreviousProcess->dwFlag & GPF_PAUSED);
			if ( m_PreviousProcess->m_dwFlag & _XGPF_PAUSED) 
			{
				_XDWINPRINT ("ResumeProcess: call %s's UnPauseLoop", m_PreviousProcess->m_processname);
				m_PreviousProcess->UnPauseProcess();
			}
			m_PreviousProcess->m_dwFlag &= ~_XGPF_PAUSED;
			m_NProcesses=m_PreviousProcess->m_StackPlace;
		}
	}
	else 
	{
		_XDWINPRINT ("ResumeProcess: no Previous process. calling ChangeProcess");
		ChangeProcess(NULL);
	}

	_XDWINPRINT ("ResumeProcess: ");
	if (m_CurrProcess) 
	{
		_XDWINPRINT ("%s ", m_CurrProcess->m_processname ? m_CurrProcess->m_processname : "null");
	}
	else
	{
		_XDWINPRINT ("NULL");
	}
	m_CurrProcess=m_PreviousProcess;

	if( m_CurrProcess )
	{
		_XDWINPRINT (" --> %s", m_CurrProcess->m_processname ? m_CurrProcess->m_processname : "null");
	}
	else
	{
		_XDWINPRINT ("NULL");
	}

	//Unlock();
}

void _XProcessManager::ResumeProcessCloseAll(_XProcess* process)
{
	//Lock();
	_XDWINPRINT ("ResumeProcessCloseAll");
	if( process==NULL ) 
	{
		ResumeProcess( process );
	}
	else 
	{
		int StackPlace = process->m_StackPlace;
		if ( m_CurrProcess->m_dwFlag & _XGPF_INITED)
			m_CurrProcess->DestroyProcess();
		
		m_CurrProcess->m_dwFlag &= ~_XGPF_INITED;
		
		m_NProcesses--;
		for ( ; m_NProcesses>StackPlace; m_NProcesses--) 
		{
			if (m_ProcessStack[m_NProcesses] && 
				m_ProcessStack[m_NProcesses]->m_dwFlag & _XGPF_INITED ) 
			{
				m_ProcessStack[m_NProcesses]->DestroyProcess();
			}
			m_ProcessStack[m_NProcesses]->m_dwFlag &= ~_XGPF_INITED;
		}
	}

	if (m_ProcessStack[m_NProcesses]->m_dwFlag & _XGPF_PAUSED)
		m_ProcessStack[m_NProcesses]->UnPauseProcess();
	m_ProcessStack[m_NProcesses]->m_dwFlag &= ~_XGPF_PAUSED;

	m_CurrProcess = m_PreviousProcess = process;
	//Unlock();
}

void _XProcessManager::CloseAllProcess()
{
	//Lock();
	if( m_CurrProcess ) 
	{
		if(m_CurrProcess->m_dwFlag & _XGPF_INITED)
		{
			m_CurrProcess->DestroyProcess();
			_XDWINPRINT ("Close all process : %s destroyed", m_CurrProcess->m_processname );
		}
		m_CurrProcess->m_dwFlag &= ~_XGPF_INITED;
	}
	
	for (m_NProcesses=m_NProcesses-1; m_NProcesses>=0; m_NProcesses--) 
	{
		if (m_ProcessStack[m_NProcesses] && m_ProcessStack[m_NProcesses]->m_dwFlag & _XGPF_INITED )
		{
			m_ProcessStack[m_NProcesses]->DestroyProcess();
			_XDWINPRINT ("Close all process : %s destroyed", m_ProcessStack[m_NProcesses]->m_processname );
		}

		m_ProcessStack[m_NProcesses]->m_dwFlag &= ~_XGPF_INITED;
	}
	
	m_CurrProcess			= NULL;		
	m_PreviousProcess		= NULL;
	m_NProcesses			= 0; 
	m_bForceChangeProcess	= 0;
	memset( m_ProcessStack, 0, sizeof(_XProcess*) * _MAXACTIVEPROCESSES );
	//Unlock();
}

bool _XProcessManager::InitProcessDeviceObject( void )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) return m_CurrProcess->InitDeviceObject();
	else return true;
}

void _XProcessManager::ReleaseProcessDeviceObject( void )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->ReleaseDeviceObject();
}

void _XProcessManager::OnPaintHandler( void )
{	
	if(m_CurrProcess && m_CurrProcess->m_Initialized && gApp && !gApp->m_bMinimized ) m_CurrProcess->Draw();		
}

bool _XProcessManager::OnKeyboardPoll(_XInput *pInput)
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) return m_CurrProcess->OnKeyboardPoll( pInput );	
	return true;
}

bool _XProcessManager::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) return m_CurrProcess->OnKeyDown( wparam, lparam );
	return true;
}

bool _XProcessManager::OnKeyUp(WPARAM wparam, LPARAM lparam)
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) return m_CurrProcess->OnKeyUp( wparam, lparam );
	return true;
}

bool _XProcessManager::OnMousePoll(MouseState *pState, ScrnPos *pPos)
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) return m_CurrProcess->OnMousePoll( pState, pPos );
	return true;
}

void _XProcessManager::OnMouseMove( WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnMouseMove( wparam, lparam );
}

bool _XProcessManager::OnMouseButton(_XMouseButton p_MB, bool bPushed)
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) return m_CurrProcess->OnMouseButton( p_MB, bPushed );
	return true;
}

void _XProcessManager::OnMouseWheel( short zDelta )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnMouseWheel( zDelta );
}

void _XProcessManager::OnMouseLBtnDblClick( WPARAM wparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnMouseLBtnDblClick(wparam);
}

void _XProcessManager::OnMouseRBtnDblClick( WPARAM wparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnMouseRBtnDblClick(wparam);
}

void _XProcessManager::OnMouseMBtnDblClick( WPARAM wparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnMouseMBtnDblClick(wparam);
}

void _XProcessManager::OnMessageHandler( UINT message, WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->MessageProc( message, wparam, lparam );
}

void _XProcessManager::OnUserMessage_Socket(  UINT message, WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess /*&& m_CurrProcess->m_Initialized*/ ) m_CurrProcess->OnUserMessage_Socket( message, wparam, lparam );
	else
	{
		_XDWINPRINT( "WARNING : no target / read socket message ![ %s ]", (m_CurrProcess) ? m_CurrProcess->m_processname : _T("Null") );
	}
}

void _XProcessManager::OnUserMessage_GetHost( UINT message, WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnUserMessage_GetHost( message, wparam, lparam );
}

void _XProcessManager::OnUserMessage_UDPSocket(  UINT message, WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnUserMessage_UDPSocket( message, wparam, lparam );	
}

void _XProcessManager::OnUserMessage_MessengerSocket(  UINT message, WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnUserMessage_MessengerSocket( message, wparam, lparam );	
}

void _XProcessManager::OnMCINotify( UINT message, WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnMCINotify( message, wparam, lparam );
}

void _XProcessManager::OnWindowSizeChangeNotify( void )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnWindowSizeChangeNotify();
}


void _XProcessManager::OnTimer( WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnTimer( wparam, lparam );
}

void _XProcessManager::OnPaint( WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized && gApp && !gApp->m_bMinimized )
		//m_CurrProcess->OnPaint( wparam, lparam );
		gApp->Render3DEnvironment();
}

void _XProcessManager::OnNexonMessangerEvent( WPARAM wparam, LPARAM lparam )
{
	if(m_CurrProcess && m_CurrProcess->m_Initialized ) m_CurrProcess->OnNexonMessangerEvent( wparam, lparam );
}
