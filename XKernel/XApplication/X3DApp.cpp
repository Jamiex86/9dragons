#include "stdafx.h"
#include "X3DApp.h"
#include "resource.h"

#include "../../NexonADBalloonLib/ADBalloon.h"

extern LRESULT		CALLBACK _XKernelWindowProc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam);

extern void			FPSProc(void);
static void			_XLogDeviceInfos(void);

#define ID_REGSTR_SCREENMODE							  "_xkern_screenmode"
#define ID_REGSTR_SCREENWIDTH							  "_xkern_screenwidth"
#define ID_REGSTR_SCREENHEIGHT							  "_xkern_screenheight"
#define ID_REGSTR_SCREENREFRESHRATE						  "_xkern_screenrefreshrate"

//#define _XENABLE_HACKSHEILD

#ifdef _XTAIWANESE
	//#define _XENABLE_GAMEGUARD
#else

	#ifdef _XVIETNAMESE
		//#define _XENABLE_GAMEGUARD
	#else
		#ifdef _ACCLAIM_VERSION
			#define _XENABLE_GAMEGUARD
		#else
			#ifdef _PWC_VERSION

			#else
				#ifdef _XJAPANESE
					#define _XENABLE_GAMEGUARD
				#else
					#ifdef _XRUSSIAN
							
					#else
						#define _XENABLE_GAMEGUARD
					#endif
				#endif
			#endif
		#endif
	#endif

#endif

#ifdef _XUSEAPEXMODULE
	#undef _XENABLE_GAMEGUARD
#endif

#if !defined(_XDWDEBUG) && !defined(_XADMINISTRATORMODE) && !defined(_DEBUG) && !defined(_XGMCLIENT) && !defined(_XPLAYER_NO_LEGACY_ANTICHEAT)
	#ifdef _XENABLE_HACKSHEILD
		#define _XUSE_HACKSHEILD
	#else
		#ifdef _XENABLE_GAMEGUARD
			#define _XUSE_GAMEGUARD
			#include "NPGameLib.h"
			#pragma message( "Include NPGameLib.h : XApp" )
			extern CNPGameLib	g_GameGuardInstance;
		#endif
	#endif
#endif

#ifdef _XDWDEBUG	
	BOOL g_MouseLockFlag = FALSE;
#endif

//#define _XKERNEL_KEYHOOK	
	
#ifdef _XKERNEL_KEYHOOK	
	
typedef struct tagKBDLLHOOKSTRUCT
{
	DWORD	vkCode;
	DWORD	scanCode;
	DWORD	flags;
	DWORD	time;
	ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;

HHOOK	g_hKeyboardHook = NULL;

void  DisableTaskSwitching( void )
{
	SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, &g_nPreviousState, 0); 
}

void  EnableTaskSwitching( void )
{
	SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, &g_nPreviousState, 0); 
}

LRESULT CALLBACK _XLowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode == HC_ACTION)
	{
		switch(wParam)
		{
		case WM_KEYUP:
		case WM_KEYDOWN:
			{
				PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
				switch(p->vkCode)
				{
				case 0x1b:	// esc key
				case 0x5b:	// left 윈도키
				case 0x5c:	// right 윈도키
				case 0x5d:	// right 메뉴키
					return 1;
				}
			}
			break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			return 1;
		}
	}
	
	return CallNextHookEx (g_hKeyboardHook, nCode, wParam, lParam); 
}
	
	
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// Name: X3DApp::X3DApp()
// Desc: Constructor.
X3DApp::X3DApp()
{
	m_bDone = false;
	m_bDeviceInitialised = false;	

	m_bWindowed         = true;   
    m_bDeviceLost       = false;
    m_bMinimized        = false;
    m_bMaximized        = false;
    m_bIgnoreSizeChange = false;
    m_bDeviceObjectsInited = false;
    m_bDeviceObjectsRestored = false;
    m_dwCreateFlags     = 0;
	m_bHasFocus			= FALSE;
	
    m_bFrameMoving      = true;
    m_bSingleStep       = false;
    m_strDeviceStats[0] = _T('\0');
    m_strFrameStats[0]  = _T('\0');

	m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
    m_bShowCursorWhenFullscreen = true;    
#ifdef _XTS_ITEMMALLBROWSER
	m_bStartFullscreen  = false;
#else
	m_bStartFullscreen  = true;
#endif
	
	memset( m_strWindowTitle, 0, sizeof(TCHAR)*64 );
	strcpy( m_strWindowTitle, _T("TripleX Kernel") );	

	g_reg_screenwidth =  gnWidth  = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENWIDTH, 1024, TRUE );
	g_reg_screenheight = gnHeight = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENHEIGHT, 768, TRUE );

	g_reg_windowedmode = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENMODE, 0, TRUE );
	g_reg_screenrefreshrate = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENREFRESHRATE, 60, TRUE );

	Pause( true ); // Pause until we're ready to render
	
    // When m_bClipCursorWhenFullscreen is true, the cursor is limited to
    // the device window when the app goes fullscreen.  This prevents users
    // from accidentally clicking outside the app window on a multimon system.
    // This flag is turned off by default for debug builds, since it makes 
    // multimon debugging difficult.
#if defined(_DEBUG) || defined(DEBUG)
    m_bClipCursorWhenFullscreen = false;
#else
    m_bClipCursorWhenFullscreen = true;
#endif
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Name: X3DApp::~X3DApp()
// Desc: Destructor.
X3DApp::~X3DApp()
{	
	m_bDone = true;	
	//_XLog( _T("Shutdown from destructor") );
	//Shutdown();
}

//-----------------------------------------------------------------------------
// Name: ConfirmDeviceHelper()
// Desc: Static function used by D3DEnumeration
//-----------------------------------------------------------------------------
bool X3DApp::ConfirmDeviceHelper( D3DCAPS9* pCaps, VertexProcessingType vertexProcessingType, 
								  D3DFORMAT backBufferFormat )
{
    DWORD dwBehavior;  
	
    if (vertexProcessingType == SOFTWARE_VP)
        dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == MIXED_VP)
        dwBehavior = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (vertexProcessingType == HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (vertexProcessingType == PURE_HARDWARE_VP)
        dwBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        dwBehavior = 0; // TODO: throw exception
    	
    return SUCCEEDED( gApp->ConfirmDevice( pCaps, dwBehavior, backBufferFormat ) );
}

void X3DApp::CheckDeviceConfirm( void )
{
	if( !gpDev )
	{
		g_bCanDoMultitexture = FALSE;
		g_bCanDoAddSigned    = FALSE;
		g_bCanDoMultitexture = FALSE;
		g_bCanDoAlphaBlend   = FALSE;
		g_bCanDoRangeFog	 = FALSE;
		g_bCanDoTableFog	 = FALSE;
		g_bCanDoVertexFog	 = FALSE;

		return;
	}	
	
	if( g_d3dCaps.MaxSimultaneousTextures > 1 )
	{
		g_bCanDoMultitexture = TRUE;
	}
	
	if( g_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_ADDSIGNED )
	{
		g_bCanDoAddSigned = TRUE;
	}
	
	if( !(g_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE2X) )
	{
		// the device can't do mod 2x.  If we also can't do add signed,
		// we have no way to do the multitexture.
		if( !g_bCanDoAddSigned )
		{
			// turn off multitexture and just go with the one detail texture
			g_bCanDoMultitexture = FALSE;
		}
	}
	
	g_bCanDoAlphaBlend = (g_d3dCaps.SrcBlendCaps  & D3DPBLENDCAPS_SRCALPHA) &&
		                 (g_d3dCaps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA);
	
	g_bCanDoRangeFog  = ( g_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE  ) ? 1 : 0;
	g_bCanDoTableFog  = ( g_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE  ) ? 1 : 0;
	g_bCanDoVertexFog = ( g_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX ) ? 1 : 0;
}

SIZE g_PreviousResolution;
BOOL g_ChangeResolutionDrawLock = TRUE;

BOOL	_XChangeToFullScreen( DWORD width, DWORD height )
{
	   if( g_reg_windowedmode || (_XCmdLineOption("xwindow",0) != 0) || (_XCmdLineOption("xmysetting",0) != 0) )
	   {
		   return TRUE;
	   }
	
	   DEVMODE dmSettings;		   	   	   	   	   	   	   	   // Device Mode variable	   
	   memset(&dmSettings,0,sizeof(dmSettings));	  	   	   // Makes Sure Memory's Cleared
	   
	   // Get current settings -- This function fills our the settings
	   // This makes sure NT and Win98 machines change correctly
	   if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	   {
		   // Display error message if we couldn't get display settings
		   _XFatalError(_T("Could Not Enum Display Settings", ));
		   return FALSE;
	   }

	   dmSettings.dmPelsWidth	 = width;		   	   // Selected Screen Width
	   dmSettings.dmPelsHeight	 = height;	   	   	   // Selected Screen Height
	   
	   // This function actually changes the screen to full screen
	   // CDS_FULLSCREEN Gets Rid Of Start Bar.
	   // We always want to get a result from this function to check if we failed
	   int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	
	   
	   // Check if we didn't recieved a good return message From the function
	   if(result != DISP_CHANGE_SUCCESSFUL)
	   {
		   // Display the error message and quit the program
		   _XFatalError(_T("Display Mode Not Compatible"));
		   return FALSE;
	   }

	   return TRUE;
}

HRESULT X3DApp::Create( HINSTANCE hInstance )
{
    HRESULT hr;

	// Setup the OSMSG callbacks
	_XAppActive = false;
	_XMsgClose = false;

	_XLog("Initilize _X3DShell.");

	// Initialise the game library
	if(!_XStartup(GetCommandLine()))
		_XFatalError("Failed to initilize _X3DShell.");

	// remove timer functions
	_XRemoveTimer(FPSProc);

	g_SystemTimeCounter.Init();

#ifdef _XTS_ITEMMALLBROWSER
		m_bStartFullscreen  = false;
		_XFullscreen		= false;

		if( !g_reg_windowedmode || (_XCmdLineOption("xwindow",0) != 0) || (_XCmdLineOption("xmysetting",0) != 0) )
		{
			gnWidth			= g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENWIDTH, 1024, TRUE );
			gnHeight		= g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENHEIGHT, 768, TRUE );	
		}
#else

	if( !g_reg_windowedmode && ((_XCmdLineOption("xwindow",0) == 0) && (_XCmdLineOption("xmysetting",0) == 0)) )
	{
#ifdef _XDWDEBUG	
		gnWidth			= g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENWIDTH, 1024, TRUE );
		gnHeight		= g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENHEIGHT, 768, TRUE );	
#else	
		gnWidth			= g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENWIDTH, 1024, FALSE );
		gnHeight		= g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_SCREENHEIGHT, 768, FALSE );
#endif					
		//dm.Format		= D3DFMT_A8R8G8B8;
		
		m_bStartFullscreen  = true;
		_XFullscreen		= true;
	}	
	else
	{
		m_bStartFullscreen  = false;
		_XFullscreen		= false;
	}

#endif
	
#ifdef _XTS_ITEMMALLBROWSER

	DEVMODE dmSettings;		   	   	   	   	   	   	   	   // Device Mode variable	   
	memset(&dmSettings,0,sizeof(dmSettings));	  	   	   // Makes Sure Memory's Cleared

	// Get current settings -- This function fills our the settings
    // This makes sure NT and Win98 machines change correctly
    if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
	   // Display error message if we couldn't get display settings
	   _XFatalError(_T("Could Not Enum Display Settings", ));
	   return FALSE;
	}   
    g_PreviousResolution.cx = dmSettings.dmPelsWidth;
    g_PreviousResolution.cy = dmSettings.dmPelsHeight;

	if( !m_bStartFullscreen )
	{
		if( !_XChangeToFullScreen( gnWidth, gnHeight ) )
		{
			return E_FAIL;
		}
	}
#endif

	_XLog("Create the Direct3D object.");

    // Create the Direct3D object
    gpD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( gpD3D == NULL )
        return DisplayErrorMsg( D3DAPPERR_NODIRECT3D, MSGERR_APPMUSTEXIT );
	
    // Build a list of Direct3D adapters, modes and devices. The
    // ConfirmDevice() callback is used to confirm that only devices that
    // meet the app's requirements are considered.
    m_d3dEnumeration.SetD3D( gpD3D );
    m_d3dEnumeration.ConfirmDeviceCallback = ConfirmDeviceHelper;
    if( FAILED( hr = m_d3dEnumeration.Enumerate() ) )
    {
        SAFE_RELEASE( gpD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }
	
    // Unless a substitute hWnd has been specified, create a window to
    // render into
    if( gHWnd == NULL)
    {
		// Register the windows class
		TCHAR strWinName[] = "X3DKernel";
        WNDCLASSEX wndClass;		
				        		
		wndClass.cbSize = sizeof(WNDCLASSEX); 
		wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndClass.lpfnWndProc = (WNDPROC)_XKernelWindowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;	
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EMPEROROFDRAGONS));
		wndClass.hCursor = NULL;//LoadCursor( NULL, MAKEINTRESOURCE(IDC_PEACE) );
		wndClass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = strWinName;
		wndClass.hIconSm = LoadIcon(gHInst, (LPCTSTR)IDI_SMALL);
		
        if( !RegisterClassEx( &wndClass ) )
		{
			int error = GetLastError();
			_XLogError("Create() - Can't Register class !");
			return NULL;			
		}

		// Set the window's initial style
#ifdef _XTS_ITEMMALLBROWSER

		if( g_reg_windowedmode || (_XCmdLineOption("xwindow",0) != 0) || (_XCmdLineOption("xmysetting",0) != 0) )
		{
			m_dwWindowStyle = //WS_VISIBLE | WS_POPUP;
				/*WS_OVERLAPPED*/WS_POPUP | WS_CAPTION | WS_SYSMENU | /*WS_THICKFRAME |*/ WS_MINIMIZEBOX | /*WS_MAXIMIZEBOX |*/ WS_VISIBLE;		
		}
		else
		{
			m_dwWindowStyle = WS_VISIBLE | WS_POPUP;
							  //WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
		}
#else
		m_dwWindowStyle = //WS_VISIBLE | WS_POPUP;
						  WS_POPUP/*WS_OVERLAPPED*/ | WS_CAPTION | WS_SYSMENU | /*WS_THICKFRAME |*/ WS_MINIMIZEBOX /*| WS_MAXIMIZEBOX*/ | WS_VISIBLE;		
#endif

        // Set the window's initial width
        RECT rc;
        SetRect( &rc, 0, 0, gnWidth, gnHeight );
        AdjustWindowRect( &rc, m_dwWindowStyle, false ); 
		
        // Create the render window        
		m_hWnd = CreateWindowEx(WS_EX_TOPMOST, strWinName, "TripleXKernel", m_dwWindowStyle,
											   -1000, -1000, (rc.right-rc.left), (rc.bottom-rc.top),
											   NULL, NULL, gHInst, NULL);

		if(m_hWnd == NULL)
		{
			int error = GetLastError();
			_XLogError("Can't create a window.");
			return NULL;
		}
		
#ifdef _XTS_ITEMMALLBROWSER
		int defaultwinxpos = 0;
		int defaultwinypos = 0;			

		if( g_reg_windowedmode || (_XCmdLineOption("xwindow",0) != 0) || (_XCmdLineOption("xmysetting",0) != 0) )
		{
			defaultwinxpos = (GetSystemMetrics(SM_CXSCREEN)-gnWidth)/2;
			defaultwinypos = (GetSystemMetrics(SM_CYSCREEN)-gnHeight)/2;
			defaultwinxpos = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONX, defaultwinxpos );	
			defaultwinypos = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONY, defaultwinypos );
		}

		SetWindowPos(m_hWnd, HWND_NOTOPMOST, defaultwinxpos, defaultwinypos, gnWidth, gnHeight, 0);
#else
		int defaultwinxpos = 0;
		int defaultwinypos = 0;

		if( !_XFullscreen )
		{	
			defaultwinxpos = (GetSystemMetrics(SM_CXSCREEN)-gnWidth)/2;
			defaultwinypos = (GetSystemMetrics(SM_CYSCREEN)-gnHeight)/2;
			defaultwinxpos = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONX, defaultwinxpos );	
			defaultwinypos = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONY, defaultwinypos );
		}
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, defaultwinxpos, defaultwinypos, (rc.right-rc.left), (rc.bottom-rc.top), 0);
#endif

		ShowWindow(m_hWnd, SW_SHOW);
    }
	
    // The focus window can be a specified to be a different window than the
    // device window.  If not, use the device window as the focus window.
    if( gHWndFocus == NULL )
        gHWndFocus = gHWnd;
	
    // Save window properties
    m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );
	
	_XLog( "Created xwindow." );

#ifdef _XUSE_GAMEGUARD
	DWORD dwResult = g_GameGuardInstance.Init();
	
	if (dwResult != NPGAMEMON_SUCCESS)
    {
		TCHAR msg[256];
		LPCSTR lpszMsg = NULL;
		
		// '6. 주요에러코드'를 참조하여 상황에 맞는 메시지를 출력해줍니다.
		switch (dwResult)
		{
		case NPGAMEMON_ERROR_EXIST:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다.";
			else
				lpszMsg = "GameGuard is already running. Try rebooting first and executing the game again.";
			
			break;
		case NPGAMEMON_ERROR_GAME_EXIST:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임이 중복 실행 되었거나 게임가드가 이미 실행 중 입니다. 게임 종료 후 다시 실행 해 보시기 바랍니다.";
			else 
				lpszMsg = "There are multiple events of game execution or GameGuard is already running. Close the game then try again.";
			break;
		case NPGAMEMON_ERROR_INIT:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수 있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.";
			else 
				lpszMsg = "GameGuard initialization error. Try rebooting and executing the game or close the program considered to cause a collision.";
			break;
		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
		case NPGAMEMON_ERROR_NFOUND_INI:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치 해 보시기 바랍니다.";
			else
				lpszMsg = "GameGuard file does not exist or is corrupted. Please install the GameGuard setup file.";
			break;
		case NPGAMEMON_ERROR_CRYPTOAPI:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를 다시 설치 해 보시기 바랍니다.";
			else 
				lpszMsg = "Window system files might be corrupted. Please reinstall the Internet Explorer(IE).";
			break;
		case NPGAMEMON_ERROR_EXECUTE:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치 해 보시기 바랍니다.";
			else 
				lpszMsg = "Fail to run GameGuard. Please reinstall the GameGuard setup file.";
			break;
		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "불법 프로그램이 발견되었습니다. 불 필요한 프로그램을 종료한 후 다시 실행 해 보시기 바랍니다.";
			else 
				lpszMsg = "Illegal program detected. Close all the unnecessary programs before running the game.";
			break;
		case NPGMUP_ERROR_ABORT:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 개인 방화벽 설정을 조정해 보시기 바랍니다.";
			else 
				lpszMsg = "GameGuard update has been aborted. Please check the status of Internet network or personal firewall settings when unable to connect continuously.";
			break;
		case NPGMUP_ERROR_CONNECT:
		case NPGMUP_ERROR_DOWNCFG:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 업데이트 서버 접속에 실패 하였습니다. 잠시 후 재시도 해보거나, 개인 방화벽이 있다면 설정을 조정해 보시기 바랍니다.";
			else 
				lpszMsg = "Fail to connect the GameGuard update server. Please try again after a while, or check personal Firewall settings if any.";
			break;
		case NPGMUP_ERROR_AUTH:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 업데이트를 완료하지 못 했습니다. 바이러스 백신을 일시 중시 시킨 후 재시도 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정해 보시기 바랍니다.";
			else 
				lpszMsg = "Fail to complete GameGuard update. Suspend Anti-Virus program temporarily and try the game, or check the settings of PC management programs if any.";
			break;
		case NPGAMEMON_ERROR_NPSCAN:
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "바이러스 및 해킹툴 검사 모듈 로딩에 실패 했습니다. 메모리 부족이거나 바이러스에 의한 감염일 수 있습니다.";
			else 
				lpszMsg = "Failed to load virus and hacking tool scanning module. Possibly due to lack of memory or virus infection";
			break;
		default:
			// 적절한 종료 메시지 출력, UI 초기화 후라면 MessageBox 대신 게임 UI를 사용해도 됨
			if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
				lpszMsg = "게임가드 실행 중 에러가 발생하였습니다. 게임 폴더 안의 GameGuard 폴더에 있는 *.erl 파일들을 game@inca.co.kr로 첨부하여 메일 보내주시기 바랍니다.";
			else 
				lpszMsg = "Error occurred while running GameGuard. Please send all *.erl files in GameGuard folder to Game2@inca.co.kr via email.";
			break;
		}
		if( lpszMsg )
		{
			sprintf(msg, "Gameguard execute error : %lu", dwResult);
			MessageBox(gHWnd, lpszMsg, msg, MB_OK); // 첫번째 인자를 NULL로 하면 안 됨
			_XLog( "GAME GUARD ERROR : %s [%lu]", lpszMsg, dwResult );
		}
		else
		{
			sprintf(msg, "Gameguard execute error : %lu", dwResult);
			MessageBox(gHWnd, "Unknown error", msg, MB_OK); // 첫번째 인자를 NULL로 하면 안 됨
			_XLog( "GAME GUARD UNKNOWN ERROR : [%lu]", dwResult );
		}		
		
		return E_FAIL;
    }	
	
	g_GameGuardInstance.SetHwnd(gHWnd);

#endif

	_XLog( "Choose d3d settings." );
	
    if( FAILED( hr = ChooseInitialD3DSettings() ) )
    {
        SAFE_RELEASE( gpD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }
	
    // Initialize the application timer
	_XLog( "Initialize xapp timer." );
    DXUtil_Timer( TIMER_START );
	
	_XLog( "Initialize 3D environment." );

    // Initialize the 3D environment for the app
    if( FAILED( hr = Initialize3DEnvironment() ) )
    {
        SAFE_RELEASE( gpD3D );
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }
	
    // The app is ready to go
    Pause( false );

	// Install timer functions
    gnFPS = gnFrameCounter = 0;
	_XAddTimer(FPSProc, 1000, TIMER_NORMAL);

	_XLog( "XApp initialized." );
	
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FindBestWindowedMode()
// Desc: Sets up m_d3dSettings with best available windowed mode, subject to 
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool X3DApp::FindBestWindowedMode( bool bRequireHAL, bool bRequireREF )
{
    // Get display mode of primary adapter (which is assumed to be where the window 
    // will appear)
    D3DDISPLAYMODE primaryDesktopDisplayMode;
    gpD3D->GetAdapterDisplayMode(0, &primaryDesktopDisplayMode);
	
    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo* pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;
	
    for( UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++ )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
        for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
        {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
            {
                D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                if (!pDeviceCombo->IsWindowed)
                    continue;
                if (pDeviceCombo->AdapterFormat != primaryDesktopDisplayMode.Format)
                    continue;
                // If we haven't found a compatible DeviceCombo yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if( pBestDeviceCombo == NULL || 
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceCombo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
                {
                    pBestAdapterInfo = pAdapterInfo;
                    pBestDeviceInfo = pDeviceInfo;
                    pBestDeviceCombo = pDeviceCombo;
                    if( pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesBB )
                    {
                        // This windowed device combo looks great -- take it
                        goto EndWindowedDeviceComboSearch;
                    }
                    // Otherwise keep looking for a better windowed device combo
                }
            }
        }
    }
EndWindowedDeviceComboSearch:
    if (pBestDeviceCombo == NULL )
        return false;
	
    m_d3dSettings.pWindowed_AdapterInfo = pBestAdapterInfo;
    m_d3dSettings.pWindowed_DeviceInfo = pBestDeviceInfo;
    m_d3dSettings.pWindowed_DeviceCombo = pBestDeviceCombo;
    m_d3dSettings.IsWindowed = true;
    m_d3dSettings.Windowed_DisplayMode = primaryDesktopDisplayMode;
    m_d3dSettings.Windowed_Width = m_rcWindowClient.right - m_rcWindowClient.left;
    m_d3dSettings.Windowed_Height = m_rcWindowClient.bottom - m_rcWindowClient.top;
    if (m_d3dEnumeration.AppUsesDepthBuffer)
        m_d3dSettings.Windowed_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    m_d3dSettings.Windowed_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    m_d3dSettings.Windowed_MultisampleQuality = 0;
    m_d3dSettings.Windowed_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    m_d3dSettings.Windowed_PresentInterval = *(UINT*)pBestDeviceCombo->pPresentIntervalList->GetPtr(0);
    return true;
}

extern UINT ColorChannelBits( D3DFORMAT fmt );

bool X3DApp::FindDefaultFullscreenMode( bool bRequireHAL, bool bRequireREF )
{	

	// For fullscreen, default to first HAL DeviceCombo that supports the current desktop 
    // display mode, or any display mode if HAL is not compatible with the desktop mode, or 
    // non-HAL if no HAL is available
    D3DDISPLAYMODE adapterDesktopDisplayMode;
    D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
    D3DDISPLAYMODE bestDisplayMode;    
    adapterDesktopDisplayMode.Format = D3DFMT_X8R8G8B8;
    adapterDesktopDisplayMode.RefreshRate = 60;

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo* pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for( UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++ )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
        	
		gpD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );

		adapterDesktopDisplayMode.Width  = gnWidth;
		adapterDesktopDisplayMode.Height = gnHeight;

        for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
        {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
            {
                D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                bool bAdapterMatchesDesktop = (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format);
                if (pDeviceCombo->IsWindowed)
                    continue;
                // If we haven't found a compatible set yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if (pBestDeviceCombo == NULL ||
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceInfo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format && bAdapterMatchesDesktop ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB )
                {
					bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
					pBestAdapterInfo = pAdapterInfo;
					pBestDeviceInfo = pDeviceInfo;
					pBestDeviceCombo = pDeviceCombo;
					if (pDeviceInfo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB)
					{
						// This fullscreen device combo looks great -- take it
						goto EndFullscreenDeviceComboSearch;
					}
					// Otherwise keep looking for a better fullscreen device combo					
                }
            }
        }
    }
EndFullscreenDeviceComboSearch:
    if (pBestDeviceCombo == NULL)
        return false;

    // Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
    // and is as close to bestAdapterDesktopDisplayMode's res as possible
    bestDisplayMode.Width = gnWidth;
    bestDisplayMode.Height = gnHeight;
    bestDisplayMode.Format = D3DFMT_UNKNOWN;
    bestDisplayMode.RefreshRate = 0;

    for( UINT idm = 0; idm < pBestAdapterInfo->pDisplayModeList->Count(); idm++ )
    {
        D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
        if( pdm->Format != pBestDeviceCombo->AdapterFormat )
            continue;
        if( pdm->Width == bestAdapterDesktopDisplayMode.Width &&
            pdm->Height == bestAdapterDesktopDisplayMode.Height &&
            pdm->RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate )			
        {
            // found a perfect match, so stop
            bestDisplayMode = *pdm;
            break;
        }
        else if( pdm->Width == bestAdapterDesktopDisplayMode.Width &&
                 pdm->Height == bestAdapterDesktopDisplayMode.Height && 
                 pdm->RefreshRate > bestDisplayMode.RefreshRate )
        {
            // refresh rate doesn't match, but width/height match, so keep this
            // and keep looking
            bestDisplayMode = *pdm;
        }
        else if( pdm->Width == bestAdapterDesktopDisplayMode.Width )
        {
            // width matches, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
        else if( bestDisplayMode.Width == 0 )
        {
            // we don't have anything better yet, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
    }

    m_d3dSettings.pFullscreen_AdapterInfo = pBestAdapterInfo;
    m_d3dSettings.pFullscreen_DeviceInfo = pBestDeviceInfo;
    m_d3dSettings.pFullscreen_DeviceCombo = pBestDeviceCombo;
    m_d3dSettings.IsWindowed = false;
    m_d3dSettings.Fullscreen_DisplayMode = bestDisplayMode;
    if (m_d3dEnumeration.AppUsesDepthBuffer)
        m_d3dSettings.Fullscreen_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    m_d3dSettings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    m_d3dSettings.Fullscreen_MultisampleQuality = 0;
    m_d3dSettings.Fullscreen_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    m_d3dSettings.Fullscreen_PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
    return true;
}


//-----------------------------------------------------------------------------
// Name: FindBestFullscreenMode()
// Desc: Sets up m_d3dSettings with best available fullscreen mode, subject to 
//       the bRequireHAL and bRequireREF constraints.  Returns false if no such
//       mode can be found.
//-----------------------------------------------------------------------------
bool X3DApp::FindBestFullscreenMode( bool bRequireHAL, bool bRequireREF )
{
    // For fullscreen, default to first HAL DeviceCombo that supports the current desktop 
    // display mode, or any display mode if HAL is not compatible with the desktop mode, or 
    // non-HAL if no HAL is available
    D3DDISPLAYMODE adapterDesktopDisplayMode;
    D3DDISPLAYMODE bestAdapterDesktopDisplayMode;
    D3DDISPLAYMODE bestDisplayMode;
    bestAdapterDesktopDisplayMode.Width = g_reg_screenwidth;
    bestAdapterDesktopDisplayMode.Height = g_reg_screenheight;
    bestAdapterDesktopDisplayMode.Format = D3DFMT_UNKNOWN;
    bestAdapterDesktopDisplayMode.RefreshRate = 0;

    D3DAdapterInfo* pBestAdapterInfo = NULL;
    D3DDeviceInfo* pBestDeviceInfo = NULL;
    D3DDeviceCombo* pBestDeviceCombo = NULL;

    for( UINT iai = 0; iai < m_d3dEnumeration.m_pAdapterInfoList->Count(); iai++ )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)m_d3dEnumeration.m_pAdapterInfoList->GetPtr(iai);
        	
		gpD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );

        for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
        {
            D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
            if (bRequireHAL && pDeviceInfo->DevType != D3DDEVTYPE_HAL)
                continue;
            if (bRequireREF && pDeviceInfo->DevType != D3DDEVTYPE_REF)
                continue;
            for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
            {
                D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
                bool bAdapterMatchesBB = (pDeviceCombo->BackBufferFormat == pDeviceCombo->AdapterFormat);
                bool bAdapterMatchesDesktop = (pDeviceCombo->AdapterFormat == adapterDesktopDisplayMode.Format);
                if (pDeviceCombo->IsWindowed)
                    continue;
                // If we haven't found a compatible set yet, or if this set
                // is better (because it's a HAL, and/or because formats match better),
                // save it
                if (pBestDeviceCombo == NULL ||
                    pBestDeviceCombo->DevType != D3DDEVTYPE_HAL && pDeviceInfo->DevType == D3DDEVTYPE_HAL ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && pBestDeviceCombo->AdapterFormat != adapterDesktopDisplayMode.Format && bAdapterMatchesDesktop ||
                    pDeviceCombo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB )
                {
					bestAdapterDesktopDisplayMode = adapterDesktopDisplayMode;
					pBestAdapterInfo = pAdapterInfo;
					pBestDeviceInfo = pDeviceInfo;
					pBestDeviceCombo = pDeviceCombo;
					if (pDeviceInfo->DevType == D3DDEVTYPE_HAL && bAdapterMatchesDesktop && bAdapterMatchesBB)
					{
						// This fullscreen device combo looks great -- take it
						goto EndFullscreenDeviceComboSearch;
					}
					// Otherwise keep looking for a better fullscreen device combo					
                }
            }
        }
    }
EndFullscreenDeviceComboSearch:
    if (pBestDeviceCombo == NULL)
        return false;

    // Need to find a display mode on the best adapter that uses pBestDeviceCombo->AdapterFormat
    // and is as close to bestAdapterDesktopDisplayMode's res as possible
    bestDisplayMode.Width = g_reg_screenwidth;
    bestDisplayMode.Height = g_reg_screenheight;
    bestDisplayMode.Format = D3DFMT_UNKNOWN;
    bestDisplayMode.RefreshRate = 0;
    for( UINT idm = 0; idm < pBestAdapterInfo->pDisplayModeList->Count(); idm++ )
    {
        D3DDISPLAYMODE* pdm = (D3DDISPLAYMODE*)pBestAdapterInfo->pDisplayModeList->GetPtr(idm);
        if( pdm->Format != pBestDeviceCombo->AdapterFormat )
            continue;
        if( pdm->Width == bestAdapterDesktopDisplayMode.Width &&
            pdm->Height == bestAdapterDesktopDisplayMode.Height && 
            pdm->RefreshRate == bestAdapterDesktopDisplayMode.RefreshRate )
        {
            // found a perfect match, so stop
            bestDisplayMode = *pdm;
            break;
        }
        else if( pdm->Width == bestAdapterDesktopDisplayMode.Width &&
                 pdm->Height == bestAdapterDesktopDisplayMode.Height && 
                 pdm->RefreshRate > bestDisplayMode.RefreshRate )
        {
            // refresh rate doesn't match, but width/height match, so keep this
            // and keep looking
            bestDisplayMode = *pdm;
        }
        else if( pdm->Width == bestAdapterDesktopDisplayMode.Width )
        {
            // width matches, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
        else if( bestDisplayMode.Width == 0 )
        {
            // we don't have anything better yet, so keep this and keep looking
            bestDisplayMode = *pdm;
        }
    }

    m_d3dSettings.pFullscreen_AdapterInfo = pBestAdapterInfo;
    m_d3dSettings.pFullscreen_DeviceInfo = pBestDeviceInfo;
    m_d3dSettings.pFullscreen_DeviceCombo = pBestDeviceCombo;
    m_d3dSettings.IsWindowed = false;
    m_d3dSettings.Fullscreen_DisplayMode = bestDisplayMode;
    if (m_d3dEnumeration.AppUsesDepthBuffer)
        m_d3dSettings.Fullscreen_DepthStencilBufferFormat = *(D3DFORMAT*)pBestDeviceCombo->pDepthStencilFormatList->GetPtr(0);
    m_d3dSettings.Fullscreen_MultisampleType = *(D3DMULTISAMPLE_TYPE*)pBestDeviceCombo->pMultiSampleTypeList->GetPtr(0);
    m_d3dSettings.Fullscreen_MultisampleQuality = 0;
    m_d3dSettings.Fullscreen_VertexProcessingType = *(VertexProcessingType*)pBestDeviceCombo->pVertexProcessingTypeList->GetPtr(0);
    m_d3dSettings.Fullscreen_PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
    return true;
}

//-----------------------------------------------------------------------------
// Name: ChooseInitialD3DSettings()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT X3DApp::ChooseInitialD3DSettings()
{
    bool bFoundFullscreen = FindDefaultFullscreenMode( true, false );
							//FindDefaultFullscreenMode( true, false ); //FindBestFullscreenMode( true, false );
    bool bFoundWindowed = FindBestWindowedMode( true, false );
	
    if( m_bStartFullscreen && bFoundFullscreen )
        m_d3dSettings.IsWindowed = false;
    if( !bFoundWindowed && bFoundFullscreen )
        m_d3dSettings.IsWindowed = false;

	_XFullscreen = !m_d3dSettings.IsWindowed;
	
    if( !bFoundFullscreen && !bFoundWindowed )
        return D3DAPPERR_NOCOMPATIBLEDEVICES;
	
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: HandlePossibleSizeChange()
// Desc: Reset the device if the client area size has changed.
//-----------------------------------------------------------------------------
HRESULT X3DApp::HandlePossibleSizeChange()
{
    HRESULT hr = S_OK;
    RECT rcClientOld;
    rcClientOld = m_rcWindowClient;
	
    if( m_bIgnoreSizeChange )
        return S_OK;
	
    // Update window properties
    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );
	
    if( rcClientOld.right - rcClientOld.left !=
        m_rcWindowClient.right - m_rcWindowClient.left ||
        rcClientOld.bottom - rcClientOld.top !=
        m_rcWindowClient.bottom - m_rcWindowClient.top)
    {
        // A new window size will require a new backbuffer
        // size, so the 3D structures must be changed accordingly.
        Pause( true );
		
        gnWidth	 = m_d3dpp.BackBufferWidth  = m_rcWindowClient.right  - m_rcWindowClient.left;
        gnHeight = m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;

		g_MainViewPort.X = 0;
		g_MainViewPort.Y = 0;
		g_MainViewPort.Width = gnWidth;
		g_MainViewPort.Height = gnHeight;
		g_MainViewPort.MinZ = 0.0f;
		g_MainViewPort.MaxZ = 1.0f;
		
        if( gpDev != NULL )
        {
            // Reset the 3D environment
            if( FAILED( hr = Reset3DEnvironment() ) )
            {
                if( hr != D3DERR_OUTOFVIDEOMEMORY )
                    hr = D3DAPPERR_RESETFAILED;
                DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
            }
        }
        Pause( false );
    }

	OnWindowSizeChangeNotify();

    return hr;
}


//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT X3DApp::Initialize3DEnvironment()
{
    HRESULT hr;

    D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
    D3DDeviceInfo* pDeviceInfo = m_d3dSettings.PDeviceInfo();

    m_bWindowed = m_d3dSettings.IsWindowed;

    // Prepare window for possible windowed/fullscreen change
    AdjustWindowForChange();

    // Set up the presentation parameters
    BuildPresentParamsFromSettings();

    if( pDeviceInfo->Caps.PrimitiveMiscCaps & D3DPMISCCAPS_NULLREFERENCE )
    {
        // Warn user about null ref device that can't render anything
        DisplayErrorMsg( D3DAPPERR_NULLREFDEVICE, 0 );
    }

    DWORD behaviorFlags;
    if (m_d3dSettings.GetVertexProcessingType() == SOFTWARE_VP)
        behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    else if (m_d3dSettings.GetVertexProcessingType() == MIXED_VP)
        behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
    else if (m_d3dSettings.GetVertexProcessingType() == HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else if (m_d3dSettings.GetVertexProcessingType() == PURE_HARDWARE_VP)
        behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
    else
        behaviorFlags = 0; // TODO: throw exception

	behaviorFlags |= D3DCREATE_MULTITHREADED;

    // Create the device
    hr = gpD3D->CreateDevice( m_d3dSettings.AdapterOrdinal(), pDeviceInfo->DevType,
                               gHWndFocus, behaviorFlags, &m_d3dpp,
                               &gpDev );

    if( SUCCEEDED(hr) )
    {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
        if( m_bWindowed )
        {
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top),
                          SWP_SHOWWINDOW );
        }

        // Store device Caps
        gpDev->GetDeviceCaps( &g_d3dCaps );
        m_dwCreateFlags = behaviorFlags;
		CheckDeviceConfirm();

        // Store device description
        if( pDeviceInfo->DevType == D3DDEVTYPE_REF )
            lstrcpy( m_strDeviceStats, TEXT("REF") );
        else if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
            lstrcpy( m_strDeviceStats, TEXT("HAL") );
        else if( pDeviceInfo->DevType == D3DDEVTYPE_SW )
            lstrcpy( m_strDeviceStats, TEXT("SW") );

        if( behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
            behaviorFlags & D3DCREATE_PUREDEVICE )
        {
            if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (pure hw vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated pure hw vp)") );
        }
        else if( behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (hw vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated hw vp)") );
        }
        else if( behaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
        {
            if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
                lstrcat( m_strDeviceStats, TEXT(" (mixed vp)") );
            else
                lstrcat( m_strDeviceStats, TEXT(" (simulated mixed vp)") );
        }
        else if( behaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
        {
            lstrcat( m_strDeviceStats, TEXT(" (sw vp)") );
        }

        if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
        {
            // Be sure not to overflow m_strDeviceStats when appending the adapter 
            // description, since it can be long.  Note that the adapter description
            // is initially CHAR and must be converted to TCHAR.
            lstrcat( m_strDeviceStats, TEXT(": ") );
            const int cchDesc = sizeof(pAdapterInfo->AdapterIdentifier.Description);
            TCHAR szDescription[cchDesc];
            DXUtil_ConvertAnsiStringToGenericCch( szDescription, 
                pAdapterInfo->AdapterIdentifier.Description, cchDesc );
            int maxAppend = sizeof(m_strDeviceStats) / sizeof(TCHAR) -
                lstrlen( m_strDeviceStats ) - 1;
            _tcsncat( m_strDeviceStats, szDescription, maxAppend );
        }

        // Store render target surface desc
        LPDIRECT3DSURFACE9 pBackBuffer = NULL;
        gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();

		gnWidth  = m_d3dsdBackBuffer.Width;			
		gnHeight = m_d3dsdBackBuffer.Height;

		g_MainViewPort.X = 0;
		g_MainViewPort.Y = 0;
		g_MainViewPort.Width = gnWidth;
		g_MainViewPort.Height = gnHeight;
		g_MainViewPort.MinZ = 0.0f;
		g_MainViewPort.MaxZ = 1.0f;

        // Set up the fullscreen cursor
        if( m_bShowCursorWhenFullscreen && !m_bWindowed )
        {            
			/*
			HCURSOR hCursor;
#ifdef _WIN64
            hCursor = (HCURSOR)GetClassLongPtr( m_hWnd, GCLP_HCURSOR );
#else
            hCursor = (HCURSOR)ULongToHandle( GetClassLong( m_hWnd, GCL_HCURSOR ) );
#endif
			*/

            //D3DUtil_SetDeviceCursor( gpDev, hCursor, true );
            //gpDev->ShowCursor( true );
			::ShowCursor( TRUE );
        }

        // Confine cursor to fullscreen window
        if( m_bClipCursorWhenFullscreen )
        {
            if (!m_bWindowed )
            {
                RECT rcWindow;
                GetWindowRect( m_hWnd, &rcWindow );
                ClipCursor( &rcWindow );
            }
            else
            {
                ClipCursor( NULL );
            }
        }

        // Initialize the app's device-dependent objects        
        if( !InitDeviceObjects() )
        {
            DeleteDeviceObjects();
			_XFatalError("Failed to initialize 3D device object!");
        }
        else
        {
			GetGammaRamp();

            m_bDeviceObjectsInited = true;
            hr = RestoreDeviceObjects();
            if( FAILED(hr) )
            {
                InvalidateDeviceObjects();
            }
            else
            {
                m_bDeviceObjectsRestored = true;
                return S_OK;
            }
        }

        // Cleanup before we try again
        Cleanup3DEnvironment();
    }

    // If that failed, fall back to the reference rasterizer
    
	if( pDeviceInfo->DevType == D3DDEVTYPE_HAL )
    {
        if (FindBestWindowedMode(false, true))
        {
            m_bWindowed = true;
            AdjustWindowForChange();
            // Make sure main window isn't topmost, so error message is visible
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW );

            // Let the user know we are switching from HAL to the reference rasterizer
            DisplayErrorMsg( hr, MSGWARN_SWITCHEDTOREF );

            hr = Initialize3DEnvironment();
        }
    }
	
    return hr;
}

//-----------------------------------------------------------------------------
// Name: BuildPresentParamsFromSettings()
// Desc:
//-----------------------------------------------------------------------------
void X3DApp::BuildPresentParamsFromSettings()
{
    m_d3dpp.Windowed               = m_d3dSettings.IsWindowed;
    m_d3dpp.BackBufferCount        = 1;
    m_d3dpp.MultiSampleType        = m_d3dSettings.MultisampleType();
    m_d3dpp.MultiSampleQuality     = m_d3dSettings.MultisampleQuality();
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_COPY;//DISCARD;
    m_d3dpp.EnableAutoDepthStencil = m_d3dEnumeration.AppUsesDepthBuffer;
    m_d3dpp.hDeviceWindow          = m_hWnd;
	//m_d3dpp.Flags				   = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    if( m_d3dEnumeration.AppUsesDepthBuffer )
    {
        m_d3dpp.Flags              = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        m_d3dpp.AutoDepthStencilFormat = m_d3dSettings.DepthStencilBufferFormat();
    }
    else
    {
        m_d3dpp.Flags              = 0;
    }
	
    if( m_bWindowed )
    {
        m_d3dpp.BackBufferWidth  = m_rcWindowClient.right - m_rcWindowClient.left;
        m_d3dpp.BackBufferHeight = m_rcWindowClient.bottom - m_rcWindowClient.top;
        m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
        m_d3dpp.FullScreen_RefreshRateInHz = 0;
        m_d3dpp.PresentationInterval = m_d3dSettings.PresentInterval();
    }
    else
    {
        m_d3dpp.BackBufferWidth  = m_d3dSettings.DisplayMode().Width;
        m_d3dpp.BackBufferHeight = m_d3dSettings.DisplayMode().Height;
        m_d3dpp.BackBufferFormat = m_d3dSettings.PDeviceCombo()->BackBufferFormat;
        m_d3dpp.FullScreen_RefreshRateInHz = m_d3dSettings.Fullscreen_DisplayMode.RefreshRate;
        m_d3dpp.PresentationInterval = m_d3dSettings.PresentInterval();
    }
}

//-----------------------------------------------------------------------------
// Name: Reset3DEnvironment()
// Desc:
//-----------------------------------------------------------------------------
HRESULT X3DApp::Reset3DEnvironment()
{
    HRESULT hr;

    // Release all vidmem objects
    if( m_bDeviceObjectsRestored )
    {
        m_bDeviceObjectsRestored = false;
        InvalidateDeviceObjects();
    }
    // Reset the device
    if( FAILED( hr = gpDev->Reset( &m_d3dpp ) ) )
        return hr;

    // Store render target surface desc
    LPDIRECT3DSURFACE9 pBackBuffer;
    gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
    pBackBuffer->Release();

    // Set up the fullscreen cursor
    if( m_bShowCursorWhenFullscreen && !m_bWindowed )
    {
/*
        HCURSOR hCursor;
#ifdef _WIN64
        hCursor = (HCURSOR)GetClassLongPtr( gHWnd, GCLP_HCURSOR );
#else
        hCursor = (HCURSOR)ULongToHandle( GetClassLong( gHWnd, GCL_HCURSOR ) );
#endif
        //D3DUtil_SetDeviceCursor( gpDev, hCursor, true );
        gpDev->ShowCursor( true );
*/
		::ShowCursor( TRUE );
    }

    // Confine cursor to fullscreen window
    if( m_bClipCursorWhenFullscreen )
    {
        if (!m_bWindowed )
        {
            RECT rcWindow;
            GetWindowRect( m_hWnd, &rcWindow );
            ClipCursor( &rcWindow );
        }
        else
        {
            ClipCursor( NULL );
        }
    }

    // Initialize the app's device-dependent objects
    hr = RestoreDeviceObjects();
    if( FAILED(hr) )
    {
 
        InvalidateDeviceObjects();
        return hr;
    }
    m_bDeviceObjectsRestored = true;

    // If the app is paused, trigger the rendering of the current frame
    if( false == m_bFrameMoving )
    {
        m_bSingleStep = true;
        DXUtil_Timer( TIMER_START );
        DXUtil_Timer( TIMER_STOP );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ToggleFullScreen()
// Desc: Called when user toggles between fullscreen mode and windowed mode
//-----------------------------------------------------------------------------
HRESULT X3DApp::ToggleFullscreen()
{
    HRESULT hr;
    int AdapterOrdinalOld = m_d3dSettings.AdapterOrdinal();
    D3DDEVTYPE DevTypeOld = m_d3dSettings.DevType();

    Pause( true );
    m_bIgnoreSizeChange = true;

    // Toggle the windowed state
    m_bWindowed = !m_bWindowed;
    m_d3dSettings.IsWindowed = m_bWindowed;

    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();

    // If AdapterOrdinal and DevType are the same, we can just do a Reset().
    // If they've changed, we need to do a complete device teardown/rebuild.
    if (m_d3dSettings.AdapterOrdinal() == AdapterOrdinalOld &&
        m_d3dSettings.DevType() == DevTypeOld)
    {
        // Reset the 3D device
        BuildPresentParamsFromSettings();
        hr = Reset3DEnvironment();
    }
    else
    {
        Cleanup3DEnvironment();
        hr = Initialize3DEnvironment();
    }
    if( FAILED( hr ) )
    {
        if( hr != D3DERR_OUTOFVIDEOMEMORY )
            hr = D3DAPPERR_RESETFAILED;
        m_bIgnoreSizeChange = false;

        if( !m_bWindowed )
        {
            // Restore window type to windowed mode
            m_bWindowed = !m_bWindowed;
            m_d3dSettings.IsWindowed = m_bWindowed;
            AdjustWindowForChange();
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                        m_rcWindowBounds.left, m_rcWindowBounds.top,
                        ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                        ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                        SWP_SHOWWINDOW );

			// Store render target surface desc
			LPDIRECT3DSURFACE9 pBackBuffer = NULL;
			gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
			pBackBuffer->Release();
			
			gnWidth  = m_d3dsdBackBuffer.Width;			
			gnHeight = m_d3dsdBackBuffer.Height;			

			g_MainViewPort.X = 0;
			g_MainViewPort.Y = 0;
			g_MainViewPort.Width = gnWidth;
			g_MainViewPort.Height = gnHeight;
			g_MainViewPort.MinZ = 0.0f;
			g_MainViewPort.MaxZ = 1.0f;
        }
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

    m_bIgnoreSizeChange = false;

    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
    if( m_bWindowed )
    {
        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                      m_rcWindowBounds.left, m_rcWindowBounds.top,
                      ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                      ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                      SWP_SHOWWINDOW );

		// Store render target surface desc
        LPDIRECT3DSURFACE9 pBackBuffer = NULL;
        gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();
		
		gnWidth  = m_d3dsdBackBuffer.Width;			
		gnHeight = m_d3dsdBackBuffer.Height;

		g_MainViewPort.X = 0;
		g_MainViewPort.Y = 0;
		g_MainViewPort.Width = gnWidth;
		g_MainViewPort.Height = gnHeight;
		g_MainViewPort.MinZ = 0.0f;
		g_MainViewPort.MaxZ = 1.0f;
    }

    Pause( false );

	OnWindowSizeChangeNotify();

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: ForceWindowed()
// Desc: Switch to a windowed mode, even if that means picking a new device
//       and/or adapter
//-----------------------------------------------------------------------------
HRESULT X3DApp::ForceWindowed()
{
    HRESULT hr;

    if( m_bWindowed )
        return S_OK;

    if( !FindBestWindowedMode(false, false) )
    {
        return E_FAIL;
    }
    m_bWindowed = true;

    // Now destroy the current 3D device objects, then reinitialize

    Pause( true );

    // Release all scene objects that will be re-created for the new device
    Cleanup3DEnvironment();

    // Create the new device
    if( FAILED(hr = Initialize3DEnvironment() ) )
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );

    Pause( false );
    return S_OK;
}

HRESULT	X3DApp::RestorePrevResolution()
{
	 HRESULT hr;
    int AdapterOrdinalOld = m_d3dSettings.AdapterOrdinal();
    D3DDEVTYPE DevTypeOld = m_d3dSettings.DevType();

    Pause( true );
    m_bIgnoreSizeChange = true;

    // Toggle the windowed state
    m_bWindowed = true;
    m_d3dSettings.IsWindowed = m_bWindowed;

    // Prepare window for windowed/fullscreen change
    AdjustWindowForChange();

    // If AdapterOrdinal and DevType are the same, we can just do a Reset().
    // If they've changed, we need to do a complete device teardown/rebuild.
    if (m_d3dSettings.AdapterOrdinal() == AdapterOrdinalOld &&
        m_d3dSettings.DevType() == DevTypeOld)
    {
        // Reset the 3D device
        BuildPresentParamsFromSettings();
        hr = Reset3DEnvironment();
    }
    else
    {
        Cleanup3DEnvironment();
        hr = Initialize3DEnvironment();
    }
    if( FAILED( hr ) )
    {
        if( hr != D3DERR_OUTOFVIDEOMEMORY )
            hr = D3DAPPERR_RESETFAILED;
        m_bIgnoreSizeChange = false;
        if( !m_bWindowed )
        {
            // Restore window type to windowed mode
            m_bWindowed = !m_bWindowed;
            m_d3dSettings.IsWindowed = m_bWindowed;
            AdjustWindowForChange();
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                        m_rcWindowBounds.left, m_rcWindowBounds.top,
                        ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                        ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                        SWP_SHOWWINDOW );

			// Store render target surface desc
			LPDIRECT3DSURFACE9 pBackBuffer = NULL;
			gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
			pBackBuffer->Release();
			
			gnWidth  = m_d3dsdBackBuffer.Width;			
			gnHeight = m_d3dsdBackBuffer.Height;			

			g_MainViewPort.X = 0;
			g_MainViewPort.Y = 0;
			g_MainViewPort.Width = gnWidth;
			g_MainViewPort.Height = gnHeight;
			g_MainViewPort.MinZ = 0.0f;
			g_MainViewPort.MaxZ = 1.0f;
        }
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }

    m_bIgnoreSizeChange = false;

    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after resetting the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.
    if( m_bWindowed )
    {
		/*
        SetWindowPos( m_hWnd, HWND_NOTOPMOST,
                      m_rcWindowBounds.left, m_rcWindowBounds.top,
                      ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                      ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                      SWP_SHOWWINDOW );
					  */

		// Store render target surface desc
        LPDIRECT3DSURFACE9 pBackBuffer = NULL;
        gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
        pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
        pBackBuffer->Release();
		
		gnWidth  = m_d3dsdBackBuffer.Width;			
		gnHeight = m_d3dsdBackBuffer.Height;

		g_MainViewPort.X = 0;
		g_MainViewPort.Y = 0;
		g_MainViewPort.Width = gnWidth;
		g_MainViewPort.Height = gnHeight;
		g_MainViewPort.MinZ = 0.0f;
		g_MainViewPort.MaxZ = 1.0f;
    }

    Pause( false );

	OnWindowSizeChangeNotify();

	g_ChangeResolutionDrawLock = FALSE;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: AdjustWindowForChange()
// Desc: Prepare the window for a possible change between windowed mode and
//       fullscreen mode.  This function is virtual and thus can be overridden
//       to provide different behavior, such as switching to an entirely
//       different window for fullscreen mode (as in the MFC sample apps).
//-----------------------------------------------------------------------------
HRESULT X3DApp::AdjustWindowForChange()
{
    if( m_bWindowed )
    {
        // Set windowed-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, m_dwWindowStyle );
    }
    else
    {
        // Set fullscreen-mode style
        SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE|CS_DBLCLKS );        
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Called in to toggle the pause state of the app.
//-----------------------------------------------------------------------------
void X3DApp::Pause( bool bPause )
{
	/*_XAppActive       = bPause;

	if( _XAppActive )
		DXUtil_Timer( TIMER_START );
	else
		DXUtil_Timer( TIMER_STOP );*/
	
    static DWORD dwAppPausedCount = 0;
		
    dwAppPausedCount += ( bPause ? +1 : -1 );
    _XAppActive       = !bPause;//( dwAppPausedCount ? false : true );
	g_LocalUserKeyboardRun = 0;
		
    // Handle the first pause request (of many, nestable pause requests)
    if( bPause && ( 1 == dwAppPausedCount ) )
    {
        // Stop the scene from animating
        if( m_bFrameMoving )
            DXUtil_Timer( TIMER_STOP );
    }
	
    if( 0 == dwAppPausedCount )
    {
        // Restart the timers
        if( m_bFrameMoving )
            DXUtil_Timer( TIMER_START );
    }
}


//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
void X3DApp::Cleanup3DEnvironment()
{
    if( gpDev != NULL )
    {
        if( m_bDeviceObjectsRestored )
        {
            m_bDeviceObjectsRestored = false;
            InvalidateDeviceObjects();
        }
        if( m_bDeviceObjectsInited )
        {
            m_bDeviceObjectsInited = false;
            DeleteDeviceObjects();
        }

		RestoreGammaRamp();
		
		int gpDevLeftInstanceCount = gpDev->Release();		
        if( gpDevLeftInstanceCount > 0 )
		{
			TCHAR messagestring[256];
			sprintf( messagestring, "A D3D object has a non-zero reference count [%d]", gpDevLeftInstanceCount );
			MessageBox( NULL, (LPCTSTR)messagestring, "9 Dragons", MB_OK );
			
			_XLog( "========================================================================");
			_XLog( messagestring );
			_XLog( "========================================================================");
		}
		else
		{
			_XLog( "========================================================================");
			_XLog( "A D3D object all released" );
			_XLog( "========================================================================");
		}
        gpDev = NULL;
    }
}

//-----------------------------------------------------------------------------
// Name: DisplayErrorMsg()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
HRESULT X3DApp::DisplayErrorMsg( HRESULT hr, DWORD dwType )
{
    static bool s_bFatalErrorReported = false;
    TCHAR strMsg[512];

    // If a fatal error message has already been reported, the app
    // is already shutting down, so don't show more error messages.
    if( s_bFatalErrorReported )
        return hr;

    switch( hr )
    {
        case D3DAPPERR_NODIRECT3D:
            _tcscpy( strMsg, _T("Could not initialize Direct3D. You may\n")
                             _T("want to check that the latest version of\n")
                             _T("DirectX is correctly installed on your\n")
                             _T("system.  Also make sure that this program\n")
                             _T("was compiled with header files that match\n")
                             _T("the installed DirectX DLLs.") );
            break;

        case D3DAPPERR_NOCOMPATIBLEDEVICES:
            _tcscpy( strMsg, _T("Could not find any compatible Direct3D\n")
                             _T("devices.") );
            break;

        case D3DAPPERR_NOWINDOWABLEDEVICES:
            _tcscpy( strMsg, _T("This program cannot run in a desktop\n")
                             _T("window with the current display settings.\n")
                             _T("Please change your desktop settings to a\n")
                             _T("16- or 32-bit display mode and re-run this\n")
                             _T("program.") );
            break;

        case D3DAPPERR_NOHARDWAREDEVICE:
            _tcscpy( strMsg, _T("No hardware-accelerated Direct3D devices\n")
                             _T("were found.") );
            break;

        case D3DAPPERR_HALNOTCOMPATIBLE:
            _tcscpy( strMsg, _T("This program requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator.") );
            break;

        case D3DAPPERR_NOWINDOWEDHAL:
            _tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window.\n"));
            break;

        case D3DAPPERR_NODESKTOPHAL:
            _tcscpy( strMsg, _T("Your Direct3D hardware accelerator cannot\n")
                             _T("render into a window with the current\n")
                             _T("desktop display settings.\n"));
            break;

        case D3DAPPERR_NOHALTHISMODE:
            _tcscpy( strMsg, _T("This program requires functionality that is\n")
                             _T("not available on your Direct3D hardware\n")
                             _T("accelerator with the current desktop display\n")
                             _T("settings.\n"));
            break;

        case D3DAPPERR_MEDIANOTFOUND:
        //case HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ):
            _tcscpy( strMsg, _T("Could not load required media." ) );
            break;

        case D3DAPPERR_RESETFAILED:
            _tcscpy( strMsg, _T("Could not reset the Direct3D device." ) );
            break;

        case D3DAPPERR_NONZEROREFCOUNT:
            _tcscpy( strMsg, _T("A D3D object has a non-zero reference\n")
                             _T("count (meaning things were not properly\n")
                             _T("cleaned up).") );
            break;

        case D3DAPPERR_NULLREFDEVICE:
            _tcscpy( strMsg, _T("Warning: Nothing will be rendered.\n")
                             _T("The reference rendering device was selected, but your\n")
                             _T("computer only has a reduced-functionality reference device\n")
                             _T("installed.  Install the DirectX reference device.\n") );
            break;

        case E_OUTOFMEMORY:
            _tcscpy( strMsg, _T("Not enough memory.") );
            break;

        case D3DERR_OUTOFVIDEOMEMORY:
            _tcscpy( strMsg, _T("Not enough video memory.") );
            break;

        default:
            _tcscpy( strMsg, _T("Generic application error. Enable\n")
                             _T("debug output for detailed information.") );
    }

    if( MSGERR_APPMUSTEXIT == dwType )
    {
        s_bFatalErrorReported = true;
        _tcscat( strMsg, _T("\n\nThis program will now exit.") );
        //MessageBox( NULL, strMsg, m_strWindowTitle, MB_ICONERROR|MB_OK );
		_XFatalError( strMsg );

        // Close the window, which shuts down the app
        //if( m_hWnd )
         //   SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
    }
    else
    {
        if( MSGWARN_SWITCHEDTOREF == dwType )
            _tcscat( strMsg, _T("\n\nSwitching to the reference rasterizer,\n")
                             _T("a software device that implements the entire\n")
                             _T("Direct3D feature set, but runs very slowly.") );
		_XLog( strMsg );
        //MessageBox( NULL, strMsg, m_strWindowTitle, MB_ICONWARNING|MB_OK );
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Name: X3DApp::Run()
// Desc: Run the program.
void X3DApp::Run()
{
	bool mbLeft = false;
	bool mbMiddle = false;
	bool mbRight = false;

	gApp = this;
	m_bDone = false;	
	
	// Initialize the Gfx mode

	if( FAILED(Create(gHInst)) )
	{
		_XFatalError("Can't create triplex core");
		return;
	}
	
#ifdef _XKERNEL_KEYHOOK
	OSVERSIONINFO ver;	
	ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&ver);	
	if(ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		g_hKeyboardHook = SetWindowsHookEx(13/*WH_KEYBOARD_LL*/,(HOOKPROC)::_XLowLevelKeyboardProc, gHInst, 0);
	} 
	else
	{
		EnableTaskSwitching();
	}
#endif

	// Rebuild time data...
	g_SystemTimeCounter.Update();
	g_LocalSystemTime	= 0;
	g_fPrevFrameTime	= 0.0f;
	g_fLocalSystemTime	= 0.0f;
	g_fElapsedFrameTime = 0.0f;
	g_fElapsedFrameMilisecondTime = 0.0f;

	_XLogDeviceInfos();

	// Startup initialisation
	if(!Startup())
	{
		_XFatalError("Can't start triplex application");
		return;
	}
	
	// reinit inputs
	gpInput = new _XInput(gHWnd, gHInst, _XFullscreen);
	
	if( !gpInput->CreateKeyboard() )
	{
		_XFatalError("Can't create keyboard interface");
	}
	
#ifdef _XDEF_USEDIRECTINPUT	
	if( !gpInput->CreateMouse(gnWidth, gnHeight) )
	{
		_XFatalError("Can't create mouse interface");
	}
#endif
				
	// Now we're ready to recieve and process Windows messages.
    BOOL bGotMsg;
    MSG  msg;
    PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	while(!_XMsgClose)
	{
		//Sleep(0);
        // Use PeekMessage() if the app is active, so we can use idle time to
        // render the scene. Else, use GetMessage() to avoid eating CPU time.        
		if( _XAppActive )
			bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );
		else
			bGotMsg = ( GetMessage( &msg, NULL, 0U, 0U ) != 0 );

		if(bGotMsg)
		{
			TranslateMessage(&msg);
			if(msg.message==WM_KEYDOWN || msg.message==WM_KEYUP)
			{
				if(msg.wParam == VK_LWIN)
					continue;
				if(msg.wParam == VK_RWIN)
					continue;
			}
			DispatchMessage(&msg);

			if( m_bMinimized )
			{
				// Rebuild time data...
				g_SystemTimeCounter.Update();
				g_LocalSystemTime	= timeGetTime();
				g_fPrevFrameTime	= g_fLocalSystemTime;
				g_fElapsedFrameTime = g_SystemTimeCounter.GetElapsedTime(); 
				g_fElapsedFrameMilisecondTime = g_SystemTimeCounter.GetElapsedMiliSecondTime();// ElapsedMilisecondTime은 1/1000초
				g_fLocalSystemTime	  += g_fElapsedFrameTime;
				g_fLocalSystemOrgTime += g_fElapsedFrameTime;

				Process();			

				++gnFrameCounter;
			}
		}
		else
		{
			// The application is in background mode so
			// we don't bother update the application
			//if(!_XAppActive) continue;

			/*
			// Get the app's time, in seconds. Skip rendering if no time elapsed
				FLOAT fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );
				FLOAT fElapsedAppTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );
				if( 0.0f == fElapsedAppTime )
				{
					continue;
				}
			*/

			// Rebuild time data...
			g_SystemTimeCounter.Update();
			g_LocalSystemTime	= timeGetTime();
			g_fPrevFrameTime	= g_fLocalSystemTime;
			g_fElapsedFrameTime = g_SystemTimeCounter.GetElapsedTime(); 
			g_fElapsedFrameMilisecondTime = g_SystemTimeCounter.GetElapsedMiliSecondTime();// ElapsedMilisecondTime은 1/1000초
			g_fLocalSystemTime	  += g_fElapsedFrameTime;
			g_fLocalSystemOrgTime += g_fElapsedFrameTime;

			if( m_bDeviceLost )
            {
                // Yield some CPU time to other processes
                Sleep( 100 ); // 100 milliseconds
				_XLog( "Device lost, Sleep 100ms");
            }

			if(gApp->m_bHasFocus)
			{
				///// Polling /////
				_XPollTimer();

#ifdef _XDWDEBUG	
				extern BOOL g_MouseLockFlag;
				if( !g_MouseLockFlag )
				{
#endif
				gpInput->Update();

#ifdef _XDWDEBUG	
				} // mouse lock
#endif			
				///// Keyboard input /////
				if(!KeyboardPoll(gpInput)) 
				{
					_XFatalError("Keyboard polling error");
					m_bDone = true;
				}
							
#ifdef _XDEF_USEDIRECTINPUT
				///// Mouse Input /////
				MouseState *pState = gpInput->GetMouseState();
				ScrnPos *pPos = gpInput->GetMouseVirScreenPos();

#ifdef _XDWDEBUG	
				if( !g_MouseLockFlag )
				{
#endif
				if(!MousePoll(pState, pPos))
					m_bDone = true;
				
				// left button
				if(pState->bButton[0])
				{
					if(!mbLeft)
					{
						mbLeft = true;
						if(!MouseButton(_XmbLeft, true))
							m_bDone = true;
					}
				}
				else
				{
					if(mbLeft)
					{
						mbLeft = false;
						if(!MouseButton(_XmbLeft, false))
							m_bDone = false;
					}
				}
				
				// right button
				if(pState->bButton[1])
				{
					if(!mbRight)
					{
						mbRight = true;
						if(!MouseButton(_XmbRight, true))
							m_bDone = true;
					}
				}
				else
				{
					if(mbRight)
					{
						mbRight = false;
						if(!MouseButton(_XmbRight, false))
							m_bDone = false;
					}
				}
				
				// middle button
				if(pState->bButton[2])
				{
					if(!mbMiddle)
					{
						mbMiddle = true;
						if(!MouseButton(_XmbMiddle, true))
							m_bDone = true;
					}
				}
				else
				{
					if(mbMiddle)
					{
						mbMiddle = false;
						if(!MouseButton(_XmbMiddle, false))
							m_bDone = false;
					}
				}
#ifdef _XDWDEBUG	
				} // mouse lock
#endif

#endif
			}
						
			// Frame move the scene
			if(!Process()) 
			{
				m_bDone = true;
			}			

			if(m_bHasFocus)
			{
				if( FAILED( Render3DEnvironment() ) ) 
					m_bDone = true;
			}
			else
			{
				//if( !g_ChangeResolutionDrawLock ) 
					Sleep( 1000 ); // 다른 프로세스에 우선권 부여.

				if( !m_bMinimized ) // 최소화 되지 않았을 경우 1초에 한번씩 렌더링.
				{
					if( FAILED( Render3DEnvironment() ) ) 
						m_bDone = true;
				}
			}

			++gnFrameCounter;
		}

		if(m_bDone)
		{
			_XMsgClose = true;
		}
	}

	m_bDone = true;

	SetForegroundWindow(GetDesktopWindow());

	// Release the device objects and Shutdown the application	

	SAFE_DELETE( gpInput );

#ifdef _XDEF_NEXONENDINGADVERTISE_071213_KERYGMA
	ADBallonParam adballonparam;
	//memset( &adballonparam, 0, sizeof(ADBallonParam) );
	strcpy( adballonparam.URL, _T("http://ad.nexon.com/NetInsight/html/nexon/nine.nexon.com/ending@middle") );
	adballonparam.width = 490;
	adballonparam.height = 190;
	adballonparam.hInst = gHInst;
	adballonparam.timeout = 60;
	
	ShowADBalloon( adballonparam );	
#endif
	
	_XLog( _T("Shutdown from main loop.") );
	Shutdown();
	
	Cleanup3DEnvironment();
	SAFE_RELEASE( gpD3D );

#ifdef _XKERNEL_KEYHOOK	
	if(ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if(g_hKeyboardHook)
		{
			if( UnhookWindowsHookEx( g_hKeyboardHook ) == 0 )
			{
				_XLog( "ERROR : Can't release keyboard hooking." );
			}
			g_hKeyboardHook = NULL;
		}
	}
	else
	{
		DisableTaskSwitching();
	}
#endif

	gApp = NULL;
}

//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene.
//-----------------------------------------------------------------------------
HRESULT X3DApp::Render3DEnvironment( void )
{
    HRESULT hr;
	
    if( m_bDeviceLost )
    {
        // Test the cooperative level to see if it's okay to render
        if( FAILED( hr = gpDev->TestCooperativeLevel() ) )
        {
            // If the device was lost, do not render until we get it back
            if( D3DERR_DEVICELOST == hr )
                return S_OK;
			
            // Check if the device needs to be reset.
            if( D3DERR_DEVICENOTRESET == hr )
            {
                // If we are windowed, read the desktop mode and use the same format for
                // the back buffer
                if( m_bWindowed )
                {
                    D3DAdapterInfo* pAdapterInfo = m_d3dSettings.PAdapterInfo();
                    gpD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &m_d3dSettings.Windowed_DisplayMode );
                    m_d3dpp.BackBufferFormat = m_d3dSettings.Windowed_DisplayMode.Format;
                }
				
                if( FAILED( hr = Reset3DEnvironment() ) )
                    return hr;
            }
            return hr;
        }
        m_bDeviceLost = false;
    }

	// Render the scene as normal
	if(!Render()) return E_FAIL;
		        	
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Name: X3DApp::SetTitle()
// Desc: Change the application title.
void X3DApp::SetTitle(TCHAR *title)
{	
	memset( m_strWindowTitle, 0, sizeof(TCHAR)*64 );
	strcpy( m_strWindowTitle, title );
	SetWindowText(gHWnd, m_strWindowTitle);
}

//-----------------------------------------------------------------------------
// Name: UserSelectNewDevice()
// Desc: Displays a dialog so the user can select a new adapter, device, or
//       display mode, and then recreates the 3D environment if needed
//-----------------------------------------------------------------------------
HRESULT X3DApp::UserSelectNewDevice()
{
    HRESULT hr;
	
    // Can't display dialogs in fullscreen mode
    if( m_bWindowed == false )
    {
        if( FAILED( ToggleFullscreen() ) )
        {
            DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
            return E_FAIL;
        }
    }
	
    _X3DSettingsDialog settingsDialog( &m_d3dEnumeration, &m_d3dSettings);
    if( settingsDialog.ShowDialog( m_hWnd ) != IDOK )
        return S_OK;
    settingsDialog.GetFinalSettings( &m_d3dSettings );
	
    m_bWindowed = m_d3dSettings.IsWindowed;
	
    // Release all scene objects that will be re-created for the new device
    Cleanup3DEnvironment();
	
    // Inform the display class of the change. It will internally
    // re-create valid surfaces, a d3ddevice, etc.
    if( FAILED( hr = Initialize3DEnvironment() ) )
    {
        if( hr != D3DERR_OUTOFVIDEOMEMORY )
            hr = D3DAPPERR_RESETFAILED;
        if( !m_bWindowed )
        {
            // Restore window type to windowed mode
            m_bWindowed = !m_bWindowed;
            m_d3dSettings.IsWindowed = m_bWindowed;
            AdjustWindowForChange();
            SetWindowPos( m_hWnd, HWND_NOTOPMOST,
				m_rcWindowBounds.left, m_rcWindowBounds.top,
				( m_rcWindowBounds.right - m_rcWindowBounds.left ),
				( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
				SWP_SHOWWINDOW );
        }
        return DisplayErrorMsg( hr, MSGERR_APPMUSTEXIT );
    }
	
    // If the app is paused, trigger the rendering of the current frame
    if( false == m_bFrameMoving )
    {
        m_bSingleStep = true;
        DXUtil_Timer( TIMER_START );
        DXUtil_Timer( TIMER_STOP );
    }
    return S_OK;
}

// system gamma table
unsigned short g_SystemGammaTable[256*3];
static D3DGAMMARAMP *g_pSystemGamma = (D3DGAMMARAMP*)&g_SystemGammaTable[0];

BOOL X3DApp::GetGammaRamp(void)
{
	if( _XFullscreen )
	{
		if( !gpDev ) return FALSE;

		if( g_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA )
		{
			gpDev->GetGammaRamp(0, g_pSystemGamma);
		}
	}
	else
	{
		//gpDev->GetGammaRamp(0, g_pSystemGamma);
		HDC pDC = GetDC(gHWnd);
		GetDeviceGammaRamp( pDC, g_pSystemGamma );
		ReleaseDC(gHWnd,pDC);
	}
	return TRUE;
}

template<class Type>
__forceinline Type Clamp( const Type x, const Type dnlimit, const Type uplimit)
{
	return ( x>=dnlimit ? (x<=uplimit ? x : uplimit): dnlimit );
}

void X3DApp::ApplyGammaRamp(void)
{
	BOOL usegamma = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, "_xkern_usegamma", TRUE, TRUE );
	if( !usegamma ) return;

	// clamp values
	g_fBrightness		= Clamp( g_fBrightness, -0.8f, 0.8f);
	g_fContrast			= Clamp( g_fContrast,    0.2f, 4.0f);
	g_fGamma			= Clamp( g_fGamma,  0.5f, 2.0f);
	g_iLevels			= Clamp( g_iLevels, 2L,  256L);
	g_fBiasR			= Clamp( g_fBiasR, 0.0f, 2.0f);
	g_fBiasG			= Clamp( g_fBiasG, 0.0f, 2.0f);
	g_fBiasB			= Clamp( g_fBiasB, 0.0f, 2.0f);
			
	// fill in the table
	LONG i;
	const FLOAT fLevels   = 256.0f * 256.0f/g_iLevels;
	const FLOAT f1oLevels = 1.0f / fLevels;
	const FLOAT f1oGamma  = 1.0f / g_fGamma;
	const FLOAT f1o65535  = 1.0f / 65535.0f;

	for( i=0; i<256; i++) 
	{
		FLOAT fVal = i/255.0f;
		fVal  = (FLOAT)pow(fVal,f1oGamma);                              // gamma
		fVal  = (fVal-0.5f)*g_fContrast +0.5f;                          // contrast
		fVal += g_fBrightness;                                          // brightness
		fVal  = ((LONG)(fVal*65535.0f*f1oLevels)) *fLevels *f1o65535;   // levels
		const FLOAT fR = Clamp( fVal*g_fBiasR, 0.0f, 1.0f);             // bias
		const FLOAT fG = Clamp( fVal*g_fBiasG, 0.0f, 1.0f);
		const FLOAT fB = Clamp( fVal*g_fBiasB, 0.0f, 1.0f);
		g_GammaTable[i+0]   = (unsigned short)(fR*65535);
		g_GammaTable[i+256] = (unsigned short)(fG*65535);
		g_GammaTable[i+512] = (unsigned short)(fB*65535);
	}

	if( _XFullscreen )
	{
		if( !gpDev ) return;

		if( g_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA )
		{
			gpDev->SetGammaRamp(0, D3DSGR_NO_CALIBRATION , (D3DGAMMARAMP*)&g_GammaTable[0]);
		}
	}
	else
	{
		//gpDev->SetGammaRamp(0, D3DSGR_NO_CALIBRATION , (D3DGAMMARAMP*)&g_GammaTable[0]);
		HDC pDC = GetDC(gHWnd);
		SetDeviceGammaRamp( pDC, &g_GammaTable[0] );
		ReleaseDC(gHWnd,pDC);
	}
}

void X3DApp::RestoreGammaRamp( void )
{
	if( _XFullscreen )
	{
		if( !gpDev ) return;

		if( g_d3dCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA )
		{
			gpDev->SetGammaRamp(0, D3DSGR_NO_CALIBRATION , g_pSystemGamma);
		}
	}
	else
	{
		//gpDev->SetGammaRamp(0, D3DSGR_NO_CALIBRATION , g_pSystemGamma);
		HDC pDC = GetDC(gHWnd);
		SetDeviceGammaRamp( pDC, g_pSystemGamma );
		ReleaseDC(gHWnd,pDC);
	}
}

#define TESTLOG(cap, flag) if(c->cap & flag) _XLog("  %s", #flag)
#define TESTLOGL(val)    _XLog("%s[%d]", #val, c->val)
#define TESTLOGF(val)    _XLog("%s[%.5f]", #val, c->val)

static void _XLogDeviceInfos(void)
{	
	D3DDEVICE_CREATION_PARAMETERS DevCP;
	D3DCAPS9 *c = &g_d3dCaps;
	if(gpDev)
		gpDev->GetCreationParameters(&DevCP);
		         
	_XLog("========================================================================");
	_XLog("3D Device informations");
	_XLog("========================================================================");
	if( gApp )
		_XLog("Driver    : \t%s", gApp->m_strDeviceStats ); 
	//_XLog("어댑터      : \t%d", gnCurrentAdapter ); 
	_XLog("Screen mode   : \t%s", _XFullscreen?"Fullscreen":"Windowed" );		  
	
	_XLog("--D3DCAPS --------------------------------------------------------------");
	TESTLOG(Caps, D3DCAPS_READ_SCANLINE);
	
	_XLog("--D3DCAPS2--------------------------------------------------------------");
	TESTLOG(Caps2, D3DCAPS2_CANAUTOGENMIPMAP);
	TESTLOG(Caps2, D3DCAPS2_CANCALIBRATEGAMMA);	
	TESTLOG(Caps2, D3DCAPS2_CANMANAGERESOURCE);	
	TESTLOG(Caps2, D3DCAPS2_DYNAMICTEXTURES);	
	
	_XLog("--D3DPRESENT------------------------------------------------------------");
	TESTLOG(PresentationIntervals, D3DPRESENT_INTERVAL_IMMEDIATE);
	TESTLOG(PresentationIntervals, D3DPRESENT_INTERVAL_ONE);
	TESTLOG(PresentationIntervals, D3DPRESENT_INTERVAL_TWO);
	TESTLOG(PresentationIntervals, D3DPRESENT_INTERVAL_THREE);
	TESTLOG(PresentationIntervals, D3DPRESENT_INTERVAL_FOUR);
	
	_XLog("--D3DCURSORCAPS---------------------------------------------------------");
	TESTLOG(CursorCaps, D3DCURSORCAPS_COLOR);
	TESTLOG(CursorCaps, D3DCURSORCAPS_LOWRES);
	
	_XLog("--D3DDEVCAPS------------------------------------------------------------");
	TESTLOG(DevCaps, D3DDEVCAPS_CANBLTSYSTONONLOCAL);
	TESTLOG(DevCaps, D3DDEVCAPS_CANRENDERAFTERFLIP);
	TESTLOG(DevCaps, D3DDEVCAPS_DRAWPRIMTLVERTEX);
	TESTLOG(DevCaps, D3DDEVCAPS_EXECUTESYSTEMMEMORY);
	TESTLOG(DevCaps, D3DDEVCAPS_EXECUTEVIDEOMEMORY);
	TESTLOG(DevCaps, D3DDEVCAPS_HWRASTERIZATION);
	TESTLOG(DevCaps, D3DDEVCAPS_HWTRANSFORMANDLIGHT);
	TESTLOG(DevCaps, D3DDEVCAPS_NPATCHES);
	TESTLOG(DevCaps, D3DDEVCAPS_PUREDEVICE);
	TESTLOG(DevCaps, D3DDEVCAPS_QUINTICRTPATCHES);
	TESTLOG(DevCaps, D3DDEVCAPS_RTPATCHES);
	TESTLOG(DevCaps, D3DDEVCAPS_RTPATCHHANDLEZERO);
	TESTLOG(DevCaps, D3DDEVCAPS_SEPARATETEXTUREMEMORIES);
	TESTLOG(DevCaps, D3DDEVCAPS_TEXTURENONLOCALVIDMEM);
	TESTLOG(DevCaps, D3DDEVCAPS_TEXTURESYSTEMMEMORY);
	TESTLOG(DevCaps, D3DDEVCAPS_TEXTUREVIDEOMEMORY);
	TESTLOG(DevCaps, D3DDEVCAPS_TLVERTEXSYSTEMMEMORY);
	TESTLOG(DevCaps, D3DDEVCAPS_TLVERTEXVIDEOMEMORY);
			
	_XLog("--D3DPRASTERCAPS--------------------------------------------------------");
	TESTLOG(RasterCaps, D3DPRASTERCAPS_ANISOTROPY);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_COLORPERSPECTIVE);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_DITHER);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_DEPTHBIAS);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_FOGRANGE);	
	TESTLOG(RasterCaps, D3DPRASTERCAPS_FOGTABLE);	
	TESTLOG(RasterCaps, D3DPRASTERCAPS_FOGVERTEX);		
	TESTLOG(RasterCaps, D3DPRASTERCAPS_MIPMAPLODBIAS);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_MULTISAMPLE_TOGGLE);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_SCISSORTEST);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_WBUFFER);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_WFOG);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_ZBUFFERLESSHSR);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_ZFOG);
	TESTLOG(RasterCaps, D3DPRASTERCAPS_ZTEST);	
	
	_XLog("--D3DPCMPCAPS-[z compare]-----------------------------------------------");
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_ALWAYS);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_EQUAL);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_GREATER);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_GREATEREQUAL);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_LESS);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_LESSEQUAL);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_NEVER);
	TESTLOG(ZCmpCaps, D3DPCMPCAPS_NOTEQUAL);
	
	_XLog("--D3DPCMPCAPS-[alpha compare]-------------------------------------------");
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_ALWAYS);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_EQUAL);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_GREATER);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_GREATEREQUAL);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_LESS);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_LESSEQUAL);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_NEVER);
	TESTLOG(AlphaCmpCaps, D3DPCMPCAPS_NOTEQUAL);
	
	_XLog("--D3DPBLENDCAPS-[blend compare]-----------------------------------------");
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_DESTALPHA);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_DESTCOLOR);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_INVDESTALPHA);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_INVSRCALPHA);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_ONE);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_SRCALPHA);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_SRCALPHASAT);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_SRCCOLOR);
	TESTLOG(SrcBlendCaps, D3DPBLENDCAPS_ZERO);

	_XLog("--D3DPMISCCAPS---[primitive misc]---------------------------------------");
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_MASKZ);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_CULLNONE);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_CULLCW);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_CULLCCW);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_COLORWRITEENABLE);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_CLIPPLANESCALEDPOINTS);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_CLIPTLVERTS);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_CLIPTLVERTS);	
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_TSSARGTEMP);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_BLENDOP);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_NULLREFERENCE);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_INDEPENDENTWRITEMASKS);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_PERSTAGECONSTANT);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_FOGANDSPECULARALPHA);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_SEPARATEALPHABLEND);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING);
	TESTLOG(PrimitiveMiscCaps, D3DPMISCCAPS_FOGVERTEXCLAMPED);
		
	
	_XLog("--D3DPBLENDCAPS-[dest blend]--------------------------------------------");
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_DESTALPHA);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_DESTCOLOR);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_INVDESTALPHA);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_INVSRCALPHA);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_ONE);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_SRCALPHA);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_SRCALPHASAT);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_SRCCOLOR);
	TESTLOG(DestBlendCaps, D3DPBLENDCAPS_ZERO);
					
	_XLog("--D3DPTFILTERCAPS-------------------------------------------------------");
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT);
	TESTLOG(TextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR);		
	
	_XLog("--D3DPTFILTERCAPS-[cube texture]----------------------------------------");
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT);
	TESTLOG(CubeTextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR);		
	
	_XLog("--D3DPTFILTERCAPS-[volume texture]--------------------------------------");
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFANISOTROPIC);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFGAUSSIANQUAD);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFANISOTROPIC);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFPYRAMIDALQUAD);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFGAUSSIANQUAD);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT);
	TESTLOG(VolumeTextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR);
		
	
	_XLog("--D3DPTADDRESSCAPS-[texture]--------------------------------------------");
	TESTLOG(TextureAddressCaps, D3DPTADDRESSCAPS_BORDER);
	TESTLOG(TextureAddressCaps, D3DPTADDRESSCAPS_CLAMP);
	TESTLOG(TextureAddressCaps, D3DPTADDRESSCAPS_INDEPENDENTUV);
	TESTLOG(TextureAddressCaps, D3DPTADDRESSCAPS_MIRROR);
	TESTLOG(TextureAddressCaps, D3DPTADDRESSCAPS_MIRRORONCE);
	TESTLOG(TextureAddressCaps, D3DPTADDRESSCAPS_WRAP);
	
	_XLog("--D3DPTADDRESSCAPS-[volume texture]-------------------------------------");
	TESTLOG(VolumeTextureAddressCaps, D3DPTADDRESSCAPS_BORDER);
	TESTLOG(VolumeTextureAddressCaps, D3DPTADDRESSCAPS_CLAMP);
	TESTLOG(VolumeTextureAddressCaps, D3DPTADDRESSCAPS_INDEPENDENTUV);
	TESTLOG(VolumeTextureAddressCaps, D3DPTADDRESSCAPS_MIRROR);
	TESTLOG(VolumeTextureAddressCaps, D3DPTADDRESSCAPS_MIRRORONCE);
	TESTLOG(VolumeTextureAddressCaps, D3DPTADDRESSCAPS_WRAP);
	
	_XLog("--D3DLINECAPS-----------------------------------------------------------");
	TESTLOG(LineCaps, D3DLINECAPS_ALPHACMP);
	TESTLOG(LineCaps, D3DLINECAPS_BLEND);
	TESTLOG(LineCaps, D3DLINECAPS_FOG);
	TESTLOG(LineCaps, D3DLINECAPS_TEXTURE);
	TESTLOG(LineCaps, D3DLINECAPS_ZTEST);
	
	_XLog("--D3DSTENCILCAPS--------------------------------------------------------");
	TESTLOG(StencilCaps, D3DSTENCILCAPS_DECR);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_DECRSAT);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_INCR);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_INCRSAT);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_INVERT);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_KEEP);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_REPLACE);
	TESTLOG(StencilCaps, D3DSTENCILCAPS_ZERO);
		
	_XLog("--D3DPTEXTURECAPS-------------------------------------------------------");
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_ALPHA);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_ALPHAPALETTE);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_CUBEMAP);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_CUBEMAP_POW2);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_MIPCUBEMAP);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_MIPMAP);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_MIPVOLUMEMAP);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_PERSPECTIVE);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_POW2);	
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_PROJECTED);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_SQUAREONLY);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_VOLUMEMAP);
	TESTLOG(TextureCaps, D3DPTEXTURECAPS_VOLUMEMAP_POW2);

	_XLog("--D3DTEXOPCAPS----------------------------------------------------------");
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_ADD);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_ADDSIGNED);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_ADDSIGNED2X);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_ADDSMOOTH);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BLENDCURRENTALPHA);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BLENDDIFFUSEALPHA);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BLENDFACTORALPHA);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BLENDTEXTUREALPHA);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BLENDTEXTUREALPHAPM);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BUMPENVMAP);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_BUMPENVMAPLUMINANCE);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_DISABLE);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_DOTPRODUCT3);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_LERP);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATE);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATE2X);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATE4X);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_MULTIPLYADD);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_PREMODULATE);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_SELECTARG1);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_SELECTARG2);
	TESTLOG(TextureOpCaps, D3DTEXOPCAPS_SUBTRACT);

	_XLog("--D3DPSHADECAPS---------------------------------------------------------");
	TESTLOG(ShadeCaps, D3DPSHADECAPS_ALPHAGOURAUDBLEND);
	TESTLOG(ShadeCaps, D3DPSHADECAPS_COLORGOURAUDRGB);
	TESTLOG(ShadeCaps, D3DPSHADECAPS_FOGGOURAUD);
	TESTLOG(ShadeCaps, D3DPSHADECAPS_SPECULARGOURAUDRGB);

	_XLog("--D3DFVFCAPS------------------------------------------------------------");
	TESTLOG(FVFCaps, D3DFVFCAPS_DONOTSTRIPELEMENTS);
	TESTLOG(FVFCaps, D3DFVFCAPS_PSIZE);
	TESTLOG(FVFCaps, D3DFVF_PSIZE);
	TESTLOG(FVFCaps, D3DFVFCAPS_TEXCOORDCOUNTMASK);
	
	_XLog("--D3DVTXPCAPS-----------------------------------------------------------");
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_DIRECTIONALLIGHTS);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_LOCALVIEWER);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_MATERIALSOURCE7);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_POSITIONALLIGHTS);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_TEXGEN);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_TEXGEN_SPHEREMAP);
	TESTLOG(VertexProcessingCaps, D3DVTXPCAPS_TWEENING);
		
	if(gpDev)
	{
		_XLog("D3DDEVICE_CREATION_PARAMETERS:");

		if(DevCP.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
			_XLog("\t\tD3DCREATE_HARDWARE_VERTEXPROCESSING");
		if(DevCP.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
			_XLog("\t\tD3DCREATE_MIXED_VERTEXPROCESSING");	
		if(DevCP.BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
			_XLog("\t\tD3DCREATE_SOFTWARE_VERTEXPROCESSING");
		if(DevCP.BehaviorFlags & D3DCREATE_PUREDEVICE)
			_XLog("\t\tD3DCREATE_PUREDEVICE");
		if(DevCP.BehaviorFlags & D3DCREATE_FPU_PRESERVE)
			_XLog("\t\tD3DCREATE_FPU_PRESERVE");	
		if(DevCP.BehaviorFlags & D3DCREATE_MULTITHREADED)
			_XLog("\t\tD3DCREATE_MULTITHREADED");
	}

	_XLog("------------------------------------------------------------------------");
	TESTLOGL(MaxPrimitiveCount);
	TESTLOGL(MaxVertexIndex);
	TESTLOGL(MaxStreams);
	TESTLOGL(MaxStreamStride);	
	TESTLOGF(MaxPointSize);
	TESTLOGL(MaxTextureWidth);
	TESTLOGL(MaxTextureHeight);
	TESTLOGL(MaxTextureRepeat);
	TESTLOGL(MaxTextureAspectRatio);
	TESTLOGL(MaxTextureBlendStages);
	TESTLOGL(MaxSimultaneousTextures);
	TESTLOGL(MaxVolumeExtent);
	TESTLOGL(MaxAnisotropy);
	TESTLOGL(MaxUserClipPlanes);
	TESTLOGF(GuardBandLeft);
	TESTLOGF(GuardBandTop);
	TESTLOGF(GuardBandRight);
	TESTLOGF(GuardBandBottom);
	TESTLOGF(ExtentsAdjust);
	TESTLOGL(MaxVertexBlendMatrices);
	TESTLOGL(MaxVertexBlendMatrixIndex);
	TESTLOGF(MaxVertexW);
	TESTLOGL(MaxActiveLights);
		
	_XLog("Vertex shader version [%d.%d]", D3DSHADER_VERSION_MAJOR(c->VertexShaderVersion), 
										   D3DSHADER_VERSION_MINOR(c->VertexShaderVersion) );	
	TESTLOGL(MaxVertexShaderConst);

	_XLog("Pixel shader version [%d.%d]", D3DSHADER_VERSION_MAJOR(c->PixelShaderVersion), 
										   D3DSHADER_VERSION_MINOR(c->PixelShaderVersion) );	
	TESTLOGL(PixelShader1xMaxValue);

	_XLog("==End of device caps log================================================");

#undef TESTLOG
#undef TESTLOGL
#undef TESTLOGF
}
