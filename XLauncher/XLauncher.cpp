// XLauncher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XLauncher.h"
#include "XLauncherDlg.h"
#include "Package.h"

#include <..\src\occimpl.h>
#include "custsite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
//아래 부분은 메소드 안에 쓰는게 아닙니다. 

//그냥 cpp file상단에 넣어 주세요. 

#pragma data_seg("Shared") // 데이타 세그먼트를 생성하는데 Shared란 섹션을 만들어라. 

long m_lUsageCnt=0; // Shared란 데이타 세그먼트 섹션에 들어가는 변수. 

// 반드시 초기화를 해주어야합니다. 이유는 초기화를 안할시 우리가 원하는 섹션으로 

// 가는것이 아니라 bbs섹션으로 들어가기 때문입니다. 

#pragma data_seg() 



#pragma comment(linker, "test /section:Shared,rws") //여기서 test는 exe화일 이름입니다. 

// 링커에게 이섹션은 쓰기, 읽기, 공유로 링크함을 알림 



이렇게 써주면 m_lUsageCnt변수는 공유를 하게됩니다. 

이것을 이용해서 우리의 어플리케이션이 몇개가 실행중인지를 알수있습니다. 



initinstance에서 m_IUsageCnt를 하나 증가 시키고 1크면 return FALSE; 하면 됩니다. 



m_IUsageCnt++; 

if(m_IUsageCnt > 1) 

return FALSE; 
 */

/////////////////////////////////////////////////////////////////////////////
// CXLauncherApp

BEGIN_MESSAGE_MAP(CXLauncherApp, CWinApp)
	//{{AFX_MSG_MAP(CXLauncherApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXLauncherApp construction

CXLauncherApp::CXLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CXLauncherApp::~CXLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXLauncherApp object

CXLauncherApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXLauncherApp initialization
extern CString g_LogFileName;
extern FILE*   g_Logfile;

BOOL		   g_MutexExitStatus = FALSE;

BOOL CXLauncherApp::InitInstance()
{
	// Create a custom control manager class so we can overide the site
	CCustomOccManager *pMgr = new CCustomOccManager;
	
	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer(pMgr);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

#ifdef _USE_MUTEX
	
	HANDLE hMutex = NULL;

	//_XLog( FALSE, "Create launcher mutex\n" );	
	hMutex = ::CreateMutex(NULL, TRUE, "XLauncher ver 0.9b");
	// 실행되었을 경우...
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{		
		//_XLog( FALSE, "FAILED : Create mutex[Already exists]\n" );
		g_MutexExitStatus = TRUE;
		CloseHandle( hMutex );
		return FALSE;
	}

	if( hMutex )
	{
		::ReleaseMutex( hMutex );
	}

#endif

#ifdef  _LOGFILE_
	char curdir[ 512 ];
	GetCurrentDirectory( 511, curdir );
	g_LogFileName = curdir;
	g_LogFileName += _T("\\XLauncher.log");

	g_Logfile = fopen( "XLauncher.log", "wt" );
	if(g_Logfile)
	{
		fclose( g_Logfile );
	}

	g_Logfile = NULL;	

	COleDateTime currentTime;
	currentTime = COleDateTime::GetCurrentTime();
	
	_XLog( FALSE, "=============================================================================\n" );
	_XLog( FALSE, "Startup log...\n" );
	_XLog( FALSE, "%d-%d-%d / %d:%d:%d \n", currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), 
			               currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond() );
	_XLog( FALSE, "=============================================================================\n" );		
#endif

#ifdef _SELFPATCH_

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	_XLog( FALSE, "Launcher section 2...\n" );

	// 셀프 패치를 위해서 다른 프로세스로 실행 
	if( strlen( cmdInfo.m_strFileName ) == 0 ||
		cmdInfo.m_strFileName.CompareNoCase( "kerygma@netian.com" ) != 0 )
	{	
		char ModuleFilePath[ 512 ];
		CString ModuleFileName;
		
		memset( ModuleFilePath, 0, 512 );
		GetModuleFileName( NULL, ModuleFilePath, 511 );

		ModuleFileName = ModuleFilePath;
		
		int i = 0 ; for(  i = 511; i > 0; i-- )
		{
			if( ModuleFilePath[i] == '\\' ) break;
			else ModuleFilePath[i] = NULL;
		}			

		_XLog( FALSE, "Module path : %s\n", ModuleFilePath );
				
		ModuleFileName.Replace( ModuleFilePath, "");

		CString runfilename = _T(".exe");

		// 실행한 파일과 실행할 파일 이름이 같으면 종료
		if( ModuleFileName.CompareNoCase( runfilename ) != 0 )
		{
			HANDLE      hFind;
			WIN32_FIND_DATA rFind;
			BOOL        bCheckFile = TRUE;

			hFind = FindFirstFile ( runfilename, &rFind );

			if ( INVALID_HANDLE_VALUE != hFind )
			{		
				FindClose ( hFind );
				CFileStatus filestatus;
				CFile::GetStatus( runfilename, filestatus );
				filestatus.m_attribute = 0x00; // normal file attrib setting
				CFile::SetStatus( runfilename, filestatus );
				
				TRY
				{
					if( CheckExistFile(runfilename,   TRUE) )
						::DeleteFile( runfilename );
				}
				CATCH( CFileException, e )
				{
					AfxMessageBox( "FAILED : Remove temporary launcher file" );
					return FALSE;
				}
				END_CATCH

			}
			
			BOOL bCheck = FALSE;

			TRY
			{
				_XLog( FALSE, "Create launcher module\n" );
				bCheck = CopyFile( ModuleFileName, runfilename, FALSE);
				_XLog( FALSE, "Created launcher module\n" );
			}
			CATCH( CFileException, e )
			{
				_XLog( FALSE, "FAILED : Create launcher file\n" );
				AfxMessageBox( "FAILED : Create launcher file" );
				return FALSE;
			}
			END_CATCH

			if(  bCheck )
			{			
				CFileStatus filestatus;
				CFile::GetStatus( runfilename, filestatus );
				filestatus.m_attribute = 0x02; // hidden file attrib setting
				CFile::SetStatus( runfilename, filestatus );

				_XLog( FALSE, "Run sub module\n" );
				::ShellExecute(NULL, NULL, runfilename , "kerygma@netian.com" , NULL, SW_HIDE);
			}			
		}
		
		return FALSE;
	}
	
	cmdInfo.m_strFileName = _T("");
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;

#endif

	_XLog( FALSE, "Start launcher sub module\n" );

	CXLauncherDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	_XLog( FALSE, "End of launcher sub module\n" );

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


int CXLauncherApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

#ifdef  _LOGFILE_	
	if( !g_MutexExitStatus )
	{
		COleDateTime currentTime;
		currentTime = COleDateTime::GetCurrentTime();		

		_XLog( FALSE, "=============================================================================\n" );
		_XLog( FALSE, "End of log.\n" );
		_XLog( FALSE, "%d-%d-%d / %d:%d:%d \n", currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), 
							   currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond() );		
		_XLog( FALSE, "==================================================================Code by _X=\n" );		
	}
#endif
	
	delete m_pDispOM;
	
	return CWinApp::ExitInstance();
}
