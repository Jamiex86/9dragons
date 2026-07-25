#include "stdafx.h"
/*
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
*/
#include <windowsx.h>
#include <direct.h>
#include <Shellapi.h>
#include <commctrl.h>
#include <d3dx9.h>

#include "resource.h"
#include "zmouse.h"
#include "xgamebase.h"
#include "jpeglib.h"

#include "XExternValues.h"

#include "XKernel.h"
#include "XExceptionReporter.h"

#define WM_MESSENGER_NOTIFY	 (WM_USER + 693 + 7) // Nexon login notify handler

extern void _XInsertChatString( LPSTR message, int messagemode );
extern void ConvertCommandLineToArgcArgv(TCHAR *argbuf);
extern SIZE g_PreviousResolution;
extern BOOL g_ChangeResolutionDrawLock; 
extern BOOL	_XChangeToFullScreen( DWORD width, DWORD height );
HANDLE g_XApplicationMutex = NULL;


#ifdef _DEBUG
	#define DXUTTRACE _XOutputDebugString
	extern VOID _XOutputDebugString( LPCTSTR strMsg, ... );
#else
	#define DXUTTRACE /##/
#endif

BOOL CheckExistFile( LPCSTR filename, BOOL resetAttrib )
{
	HANDLE      hFind;
	WIN32_FIND_DATA rFind;	

	hFind = FindFirstFile ( filename, &rFind );

	if ( INVALID_HANDLE_VALUE == hFind )
	{
		FindClose ( hFind );
		return FALSE;
	}

	FindClose ( hFind );

	if( resetAttrib )
	{
		SetFileAttributes( filename, FILE_ATTRIBUTE_ARCHIVE );
	}

	return TRUE;
}

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
						//#define _XENABLE_HACKSHEILD
						#define _XENABLE_GAMEGUARD
						//#define _XENABLE_XTRAP
					#endif
				#endif
			#endif
		#endif
	#endif

#endif

#ifdef _XUSEAPEXMODULE
	#undef _XENABLE_GAMEGUARD
	#pragma message( "undefine game guard : apex module." )
#endif

#if !defined(_XDWDEBUG) && !defined(_XADMINISTRATORMODE) && !defined(_DEBUG) && !defined(_XGMCLIENT) && !defined(_XPLAYER_NO_LEGACY_ANTICHEAT)
	#ifdef _XENABLE_HACKSHEILD
		#define _XUSE_HACKSHEILD
	#else
		#ifdef _XENABLE_GAMEGUARD
			#define _XUSE_GAMEGUARD
		#else
			#ifdef _XENABLE_XTRAP
				#define _XUSE_XTRAP
			#endif
		#endif
	#endif
#endif

#ifdef _XUSE_HACKSHEILD
	#include "HShield.h"
	DWORD  g_dwMainThreadID;
	extern int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
#endif

#ifdef _XUSE_GAMEGUARD
	#include "NPGameLib.h"
	#pragma message( "Include NPGameLib.h : XKernel" )
	
	#ifdef _XVIETNAMESE
		#pragma comment(lib, "./Library/NPGameLib_DE.lib")
		//#ifdef _XDEF_NPROTECT_GUARD
		//		#pragma message("VIETNAMESE_Test_GameGuard")
		//		CNPGameLib	g_GameGuardInstance( _T("CS50Test") );
		//#else
				#pragma message("VIETNAMESE GameGuard")
				CNPGameLib	g_GameGuardInstance( _T("NineDragonsVN") );
		//#endif
	#else
		#ifdef _XTAIWANESE
		
		#else
			#ifdef _ACCLAIM_VERSION
				#pragma comment(lib, "./Library/NPGameLib_SE.lib")
				//#ifdef _XDEF_NPROTECT_GUARD
				//	CNPGameLib	g_GameGuardInstance( _T("CS50Test") );
				//#else
					CNPGameLib	g_GameGuardInstance( _T("NineDragonsUS") );
					#pragma message("US")
				//#endif
			#else
				#ifdef _PWC_VERSION
								
				#else
					#ifdef _XJAPANESE
						#pragma comment(lib, "./Library/NPGameLib_SE.lib")					
						CNPGameLib	g_GameGuardInstance( _T("NineDragonsJP") );
						#pragma message("JP")					
					#else
						#ifdef _XRUSSIAN
							
						#else
							#pragma comment(lib, "./Library/NPGameLib_SE.lib")
							CNPGameLib	g_GameGuardInstance( _T("NineDragons") );
							#pragma message("ninedragons.ini")
							#pragma message("Korea")
						#endif
					#endif
				#endif
			#endif
		#endif
	#endif
#endif

#ifdef _XUSE_XTRAP
	#include "XTrap.h"
	#pragma comment(lib, "./Library/XTrapIC_mtDll.lib")
#endif

#define _XDEF_XFILETRASFER_FILENAME				"XFileTransfer.exe"

#ifdef _XTESTSERVER
#define _XDEF_LAUNCHER_FILENAME					"tsNDLauncher.exe"
#else
#define _XDEF_LAUNCHER_FILENAME					"NDLauncher.exe"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// Render state machine controller
#ifdef _XADMINISTRATORMODE
void					  SetCommandConsoleWindow( void );
void					  ShowRenderStateControlDlg( void );
extern void				 _XKernelCommandProcedure( LPSTR commandstring );
extern LRESULT	CALLBACK _XKernelCommandWindowProc( HWND hDlg, UINT message, WPARAM wparam, LPARAM lparam );
static LRESULT	CALLBACK _XKernelCommandEditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

LRESULT CALLBACK		 _XKernelWindowProc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK		 _XKernelDebugWindowProc( HWND hDlg, UINT message, WPARAM wparam, LPARAM lparam  );
void					  SetDebugConsolWindow( void );
BOOL					  SaveSurfaceToJPG( IDirect3DSurface9*& pSrcSurface, LPTSTR szFilename );

//#define _XKERNEL_KEYHOOK	

#ifdef _XKERNEL_KEYHOOK	
extern HHOOK			  g_hKeyboardHook;
extern void  DisableTaskSwitching( void );
#endif
/*

  // leak check...

  #include <crtdbg.h>
  #ifdef _DEBUG
  #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
  #endif

  int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // 현재 플래그 얻음.
  
  tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF; // Leak 체크 플래그 추가
  tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF; // 블럭들을 실제로 해제하지 않는 플래그 추가.	 
  tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF; // alloc/dealloc에 대해 메모리 점검 수행 플래그 추가.
	
  _CrtSetDbgFlag(tmpDbgFlag); // 새로운 플래그 설정.
	  
  _CrtCheckMemory(); // 메모리 체크 결과를 보여줌.
  _CrtDumpMemoryLeaks(); // 메모리 Leak 체크 결과를 보여줌.
*/


LRESULT CALLBACK _XKernelWindowProc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{	
	switch(message)
	{
		case WM_COMMAND:
			if(gApp)
				gApp->MessageHandler( message, wparam, lparam );
			break;
		case WM_USER_SOCKET:
				gApp->OnUserMessage_Socket( message, wparam, lparam );
				return TRUE;
			break;
		case WM_USER_GETHOST:
				gApp->OnUserMessage_GetHost( message, wparam, lparam );
				return TRUE;
			break;			
		case WM_USER_UDPSOCKET:
				gApp->OnUserMessage_UDPSocket( message, wparam, lparam );
				return TRUE;
			break;			
		case WM_USER_MESSENGERSOCKET: 
				gApp->OnUserMessage_MessengerSocket( message, wparam, lparam );
				return TRUE;
			break;
		
		case WM_MESSENGER_NOTIFY :
			{
				gApp->OnNexonMessangerEvent( wparam, lparam );
				return TRUE;
			}
			break;
			
		case WM_MOVE :
			{
				gApp->OnMove( wparam, lparam );
				return TRUE;
			}
			break;
		/*case WM_NCMOUSEMOVE:
			{
				POINT point;
				point.x = LOWORD(lparam);
				point.y = HIWORD(lparam);
				::ScreenToClient( gHWnd, &point );
				gpInput->SetMousePosition( point.x, point.y );
			}		
			break;*/
		case WM_MOUSEMOVE:
			{
				
#ifdef _XDWDEBUG	
				extern BOOL g_MouseLockFlag;
				if( !g_MouseLockFlag )
				{
#endif
				if( gpInput ) gpInput->SetMousePosition( LOWORD(lparam), HIWORD(lparam) );				
				if(gApp)gApp->OnMouseMove( wparam, lparam );
#ifdef _XDWDEBUG	
				}
#endif
			}
				return TRUE;
			break;
		case WM_LBUTTONDBLCLK:
			if(gApp) gApp->MouseLBtnDblClick( wparam );
			break;
		case WM_RBUTTONDBLCLK:
			if(gApp) gApp->MouseRBtnDblClick( wparam );
			break;
		case WM_MBUTTONDBLCLK:
			if(gApp) gApp->MouseMBtnDblClick( wparam );
			break;
#ifndef _XDEF_USEDIRECTINPUT
		case WM_LBUTTONDOWN:
				SetCapture( gHWnd );
				gpInput->SetMouseLButton( true );
				if(gApp) //gApp->MousePoll(gpInput->GetMouseState(), gpInput->GetMouseScreenPos());
					gApp->MouseButton(_XmbLeft, true);
				return TRUE;
			break;
		case WM_LBUTTONUP:
				ReleaseCapture();
				gpInput->SetMouseLButton( false );
				if(gApp)//gApp->MousePoll(gpInput->GetMouseState(), gpInput->GetMouseScreenPos());
					gApp->MouseButton(_XmbLeft, false);
				return TRUE;
			break;
		case WM_RBUTTONDOWN:
				SetCapture( gHWnd );
				gpInput->SetMouseRButton( true );
				if(gApp)//gApp->MousePoll(gpInput->GetMouseState(), gpInput->GetMouseScreenPos());
					gApp->MouseButton(_XmbRight, true);
				return TRUE;
			break;
		case WM_RBUTTONUP:
				ReleaseCapture();
				gpInput->SetMouseRButton( false );
				if(gApp)//gApp->MousePoll(gpInput->GetMouseState(), gpInput->GetMouseScreenPos());
					gApp->MouseButton(_XmbRight, false);
				return TRUE;
			break;
		case WM_MBUTTONDOWN:
				SetCapture( gHWnd );
				gpInput->SetMouseMButton( true );			
				if(gApp)//gApp->MousePoll(gpInput->GetMouseState(), gpInput->GetMouseScreenPos());
					gApp->MouseButton(_XmbMiddle, true);
				return TRUE;
			break;
		case WM_MBUTTONUP:
				ReleaseCapture();
				gpInput->SetMouseMButton( false );
				if(gApp)//gApp->MousePoll(gpInput->GetMouseState(), gpInput->GetMouseScreenPos());
					gApp->MouseButton(_XmbMiddle, false);
				return TRUE;
			break;
#endif

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Process IME composition...
		case WM_INPUTLANGCHANGE:
			_XIMEKERNEL.OnInputLanguageChange(wnd, wparam, lparam);
			break;
		
		case WM_IME_SETCONTEXT:
			{
				//DXUTTRACE( "WM_IME_SETCONTEXT\n" );
				lparam = 0;
			}
			break;

		case WM_IME_STARTCOMPOSITION:
			DXUTTRACE( "WM_IME_STARTCOMPOSITION\n" );
			return false;

		case WM_CHAR:				
				if( !_XIMEKERNEL.GetIMEContainer() || gpInput->CheckKeyPress(DIK_LCONTROL) || gpInput->CheckKeyPress(DIK_LMENU) )
				{						
					//return gApp->KeyDown( wparam, lparam );
				}
				else
				{
					_XIMEKERNEL.OnChar(wnd, wparam, lparam);
					//return false;
				}
				break;
		case WM_KEYDOWN:
				switch( wparam ) 
				{				
				case VK_F1		:
				case VK_F2		:
				case VK_F3		:
				case VK_F4		:
				case VK_F5		:
				case VK_F6		:
				case VK_F7		:
				case VK_F8		:
				case VK_F9		:
				case VK_F10		:
				case VK_F11		:	
				case VK_F12		:
					return gApp->KeyDown( wparam, lparam );
				case VK_UP		:
				case VK_DOWN	:
				case VK_PRIOR   :	// Page up
				case VK_NEXT	:	// Page down
					return false;
					
				default:			
					if( !_XIMEKERNEL.GetIMEContainer() || gpInput->CheckKeyPress(DIK_LCONTROL) || gpInput->CheckKeyPress(DIK_LMENU) )
					{
						return gApp->KeyDown( wparam, lparam );
					}
					else
					{
						_XIMEKERNEL.GetKey(wparam,lparam);
						
						/*
						if( _XIMEKERNEL.GetKey(wparam,lparam) )
						{
							return false;
						}
						*/
					}
				}				
				break;
		case WM_IME_COMPOSITION:							
				{
					DXUTTRACE( "WM_IME_COMPOSITION\n" );
					if( _XIMEKERNEL.OnComposition(wnd, wparam, lparam) )
					{
						return 0L;
					}					
				}
				break;
		case WM_IME_ENDCOMPOSITION :
				{
					DXUTTRACE( "WM_IME_ENDCOMPOSITION\n" );										
					if( _XIMEKERNEL.OnEndComposition(wnd, wparam, lparam) )
					{
						return 0L;
					}
				}
				break;
		case WM_IME_NOTIFY:
				{
					DXUTTRACE( "WM_IME_NOTIFY %d\n", wparam );

					if( _XIMEKERNEL.OnNotify(wnd, wparam, lparam) )
					{
						return 0L;
					}
				}
				break;
		case WM_KEYUP:
														   // 중복 입력 불가
			if( (int)wparam == VK_SNAPSHOT && (HIWORD( lparam ) & 0xFF) == 0x37 )
			{					
				_XLog( "Capture : Start backbuffer capture : locking backbuffer" );
				IDirect3DSurface9* pSrcSurface = NULL;
				
				//if( SUCCEEDED( gpDev->GetRenderTarget( 0, &pSrcSurface ) ) )
				if( SUCCEEDED( gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSrcSurface ) ) )
				//if( SUCCEEDED( gpDev->GetFrontBuffer( pSrcSurface ) ) )
				{
					if( pSrcSurface )
					{												
						_XSetDefaultPath();

						if( _chdir( _XDEF_DEFAULTPATH_CAPTURE ) == -1 )
						{
							TCHAR message[256];
							if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
								sprintf( message, "스크린샷을 저장할 기본 폴더를 생성합니다. [%s]", gCaptureFileSubFolder );
							else
								sprintf( message, "Create folder. [%s]", gCaptureFileSubFolder );
								
							_XInsertChatString( message, 11 );

							_mkdir( _XDEF_DEFAULTPATH_CAPTURE );

							if( _chdir( _XDEF_DEFAULTPATH_CAPTURE ) == -1 )
							{
								_XSetDefaultPath();
							}
						}

						if( strlen( gCaptureFileSubFolder ) > 0 )
						{
							if( _chdir( gCaptureFileSubFolder ) == -1 )
							{
								TCHAR message[256];
								if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
									sprintf( message, "스크린샷을 저장할 사용자별 개인 폴더를 생성합니다. [%s]", gCaptureFileSubFolder );
								else
									sprintf( message, "Create folder. [%s]", gCaptureFileSubFolder );
								_XInsertChatString( message, 11 );

								_mkdir( gCaptureFileSubFolder );

								if( _chdir( gCaptureFileSubFolder ) == -1 )
								{
									_XSetDefaultPath();
								}
							}
						}

						TCHAR filename[_MAX_PATH];
						time_t ltime;    
						struct tm *today;
						
						time( &ltime );
						today = localtime( &ltime );
												
						memset( filename, 0, sizeof(TCHAR) * _MAX_PATH );
						strftime( filename, 128, "%Y%B%d %H-%M-%S.jpg", today );
						
						_XLog( "Capture : Start backbuffer capture : save backbuffer" );

						if( !SaveSurfaceToJPG( pSrcSurface, filename ) )
						{
							_XDWINPRINT("WARNING : Failed screen save : %s", filename );
							TCHAR message[256];
							if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
								sprintf( message, "스크린 샷 저장에 실패하였습니다. 이용에 불편을 드려 죄송합니다.", filename );
							else
								sprintf( message, "Failed to create screen shot.", filename );
							_XInsertChatString( message, 11 );
						}

						/*memset( filename, 0, sizeof(TCHAR) * _MAX_PATH );
						strftime( filename, 128, "%Y%B%d %H-%M-%S.bmp", today );
						D3DXSaveSurfaceToFile( filename, D3DXIFF_BMP, pSrcSurface, NULL, NULL );*/

						SAFE_RELEASE( pSrcSurface );
						
						_XLog( "Capture : End back buffer capture" );

						_XDWINPRINT("NOTICE : Screen captured : %s", filename );
					}
				}
				else
				{
					_XLog( "Capture : ERROR : GetBackBuffer" );
				}
				return 1;
			}
			else
			{
				if( _XIMEKERNEL.GetIMEContainer() )
				{
					//return false;
				}
				else
				{
					return gApp->KeyUp(wparam,lparam);
				}
			}
			break;
		case WM_SYSKEYDOWN :			
#ifndef _XTS_ITEMMALLBROWSER
#ifdef _XDWDEBUG
			if( (U32)wparam == VK_RETURN )
			{				
				// Toggle the fullscreen/window mode
				gApp->Pause( true );
				if( FAILED( gApp->ToggleFullscreen() ) )
					gApp->DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
				gApp->Pause( false );

				gApp->ReleaseClickCursor();

				return TRUE;
			}
#else
			if( g_reg_windowedmode || _XCmdLineOption("xwindow",0) != 0 )
			{
				if( (U32)wparam == VK_RETURN )
				{
					gApp->Pause( true );
					if( FAILED( gApp->ToggleFullscreen() ) )
						gApp->DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
					gApp->Pause( false );
					gApp->ReleaseClickCursor();
					return TRUE;
				}
			}
#endif
#else

//#define VN_RB_RUNTIMECHANGESCREENMODE_20070525_KERYGMA

#ifdef  VN_RB_RUNTIMECHANGESCREENMODE_20070525_KERYGMA

			if( (U32)wparam == VK_RETURN )
			{
				g_ChangeResolutionDrawLock = TRUE;

				gApp->Pause( true );

				BOOL prevval = g_reg_windowedmode;

				g_reg_windowedmode = FALSE;

				SetWindowPos( gHWnd, HWND_NOTOPMOST,
								0, 0,
								g_reg_screenwidth, g_reg_screenheight,
								0 );

				::ShowWindow(wnd, SW_MINIMIZE);

				if( !_XChangeToFullScreen( g_reg_screenwidth, g_reg_screenheight) )
				{
					return 0;
				}

				g_reg_windowedmode = prevval;

				DWORD WindowStyle = WS_VISIBLE | WS_POPUP;
				SetWindowLong( gHWnd, GWL_STYLE, WindowStyle );

				::ShowWindow(wnd, SW_NORMAL);

				gApp->Pause( false );
				gApp->RestoreGammaRamp();

				g_ChangeResolutionDrawLock = FALSE;

				// Store render target surface desc
				LPDIRECT3DSURFACE9 pBackBuffer = NULL;
				gpDev->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
				pBackBuffer->GetDesc( &gApp->m_d3dsdBackBuffer );
				pBackBuffer->Release();
				
				gnWidth  = gApp->m_d3dsdBackBuffer.Width;			
				gnHeight = gApp->m_d3dsdBackBuffer.Height;			

				g_MainViewPort.X = 0;
				g_MainViewPort.Y = 0;
				g_MainViewPort.Width = gnWidth;
				g_MainViewPort.Height = gnHeight;
				g_MainViewPort.MinZ = 0.0f;
				g_MainViewPort.MaxZ = 1.0f;
			}

#endif
			/*
			if( (U32)wparam == VK_RETURN )
			{
				static BOOL _Fullmode = TRUE;

				if( _Fullmode )
				{
					if( (_XCmdLineOption("xwindow",0) == 0) && (_XCmdLineOption("xmysetting",0) == 0) )				
					{
						gApp->Pause( true );
						::ShowWindow(wnd, SW_MINIMIZE);
						if( !_XChangeToFullScreen( g_PreviousResolution.cx, g_PreviousResolution.cy ) )
						{
							return 0;
						}
						_Fullmode = FALSE;
					}		
				}
				else
				{
					if( !_XChangeToFullScreen( gnWidth, gnHeight ) )
					{
						return 0;
					}
					
					::ShowWindow(wnd, SW_SHOW);
					
					gApp->m_bMinimized = false;
					gApp->RestorePrevResolution();
										
					gApp->Pause( false );

					_Fullmode = TRUE;
				}
			}
			*/
#endif
			break;
		case WM_MOUSEWHEEL:
			if(gApp)
				gApp->MouseWheel( wparam );
			break;

		case WM_CREATE:
			{
				gHWnd = wnd;
				::SetForegroundWindow( gHWnd );

				//////////////////////////////////////////////////////////////////////////
				// IME Initialize
				g_imm.Initialize();
				_XIMEKERNEL.SetupImeApi(gHWnd);
				_XIMEKERNEL.CheckToggleState( gHWnd );
				_XIMEKERNEL.SetNativeMode( false );
				
				/*
				HIMC hImc;
				if( NULL != ( hImc = g_imm._ImmGetContext( gHWnd ) ) ) 
				{
					g_imm._ImmSetConversionStatus( hImc,  );

					g_imm._ImmReleaseContext( gHWnd, hImc );
					//_XIMEKERNEL.m_ImeState = IMEUI_STATE_OFF;
					//_XIMEKERNEL.CheckToggleState( gHWnd );
				}
				*/
			}

			//_XIMEKERNEL.OnInputLanguageChange(wnd, 0, (LPARAM)GetKeyboardLayout(0));
			
#ifdef _XDWDEBUG
			SetDebugConsolWindow();
#endif
			break;
			
		case WM_TIMER:
			if(gApp)
				gApp->OnTimer( wparam, lparam );
			break;

		case WM_PAINT:
			if(gApp)
			{
				//if( !gApp->m_bMinimized )
				//	gApp->OnPaint( wparam, lparam );
			}
			break;

			/*
			if( gApp )
			{
				if( gpDev && !_XAppActive && gApp->m_bWindowed &&
					gApp->m_bDeviceObjectsInited && gApp->m_bDeviceObjectsRestored )
					gApp->PaintHandler();
			}*/

			/*if(gApp && !_XAppActive ) 
			{
				HDC dc ;
				PAINTSTRUCT ps ;
				dc = BeginPaint( wnd, &ps ) ;

				SendMessage(wnd, WM_ICONERASEBKGND, (WPARAM) dc, 0);

				int cxIcon = GetSystemMetrics(SM_CXICON);
				int cyIcon = GetSystemMetrics(SM_CYICON);

				RECT rect;
				GetClientRect(wnd, &rect);

				int x = ((rect.right-rect.left) - cxIcon + 1) / 2;
				int y = ((rect.bottom-rect.top) - cyIcon + 1) / 2;

				// Draw the icon
				DrawIcon( dc, x, y, LoadIcon(gHInst, MAKEINTRESOURCE(IDI_EMPEROROFDRAGONS)));
				
				EndPaint( wnd, &ps );
			}*/
			break;
			
		case WM_SETCURSOR:
			// Turn off Windows cursor in fullscreen mode
			extern BOOL g_UseHardwareCursor;
			if( gApp && gpDev )
			{
				if( _XAppActive )
				{
					if( g_UseHardwareCursor )
					{
						::SetCursor( NULL );
						if( gApp->m_bShowCursorWhenFullscreen )
							gpDev->ShowCursor( true );
					}
					else
					{
						//gApp->SetCursor( 0, TRUE );
					}
				}
				else
				{
					//gApp->SetCursor( 0, TRUE );
				}
			}
			else
			{
				//if( gApp ) gApp->SetCursor( 0, TRUE );
			}
			
			return true; // prevent Windows from setting cursor to window class cursor

			break;
			
		case WM_DESTROY:			
			{
				if(gApp) gApp->RestoreGammaRamp();
				
				if( !g_NormalGameExit )
				{
					return 1;	
				}
				else
				{
					if( gApp->m_bWindowed )
					{				
#ifdef _XADMINISTRATORMODE
						RECT rect;
						GetWindowRect( gHWnd, &rect );
						g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONX, rect.left );
						g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONY, rect.top );
#endif
					}
				}
			}

#ifdef _XDWDEBUG
			if(_XDebugWindow)
			{				
				if( g_DebugWindowHandle )
				{
					DestroyWindow(g_DebugWindowHandle);
					g_DebugWindowHandle = NULL;
				}					
			}			
#endif
#ifdef _XADMINISTRATORMODE
			if( _XConsole ) 
			{
				if( g_hRenderStateControllerWindow )
				{				
					DestroyWindow(g_hRenderStateControllerWindow);
					g_hRenderStateControllerWindow = NULL;			
				}
			}
#endif
			PostQuitMessage(0);
			break;
		
		case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lparam)->ptMinTrackSize.x = 800;
            ((MINMAXINFO*)lparam)->ptMinTrackSize.y = 600;
            break;

		case WM_ENTERSIZEMOVE :
			if(gApp) 
			{
				gApp->Pause( true );				
			}
			break;

		case WM_WINDOWPOSCHANGING :
			break;
		
		case WM_WINDOWPOSCHANGED :
			{
				#ifdef _XTS_ITEMMALLBROWSER						
					if( g_reg_windowedmode || (_XCmdLineOption("xwindow",0) != 0) || (_XCmdLineOption("xmysetting",0) != 0) )
					{
						WINDOWPOS* lpwp = (LPWINDOWPOS) lparam;
							
						if( !(lpwp->flags & SWP_NOMOVE) )
						{
							extern HWND g_hItemMallWindow;
							extern HWND g_hItemMallWebBrowserObject;
							
							/*
							RECT rect;
							GetWindowRect( gHWnd, &rect );
							RECT clientrect;
							GetClientRect( gHWnd, &clientrect );
							*/

							POINT pt = { 0, 0 };
							ClientToScreen( gHWnd, &pt );

							if( g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
							{
								SetWindowPos(g_hItemMallWindow, NULL, pt.x + (gnWidth>>1)-400, pt.y + (gnHeight>>1)-300, 800, 600, 0);
							}
							else
							{
								SetWindowPos(g_hItemMallWindow, HWND_TOPMOST, pt.x + (gnWidth>>1)-400, pt.y + (gnHeight>>1)-300, 800, 600, 0);
								//SetWindowPos(g_hItemMallWebBrowserObject, HWND_TOPMOST, 0, 0, 800, 600, 0);
							}
						}
					}
				#endif
			}
			break;
			
			
		case WM_ACTIVATEAPP:

			gApp->m_bHasFocus = (BOOL)wparam;
			/*if( (BOOL)wparam )
				gApp->m_bHasFocus = (BOOL)wparam;*/
			
			if(wparam == FALSE)
			{
				
#ifdef _XTS_ITEMMALLBROWSER
				if( !gApp->m_bMinimized )
				{				
					if( g_LanguageType != _XLANGUAGE_TYPE_JAPANESE && g_LanguageType != _XLANGUAGE_TYPE_ENGLISH )
					{
						if( !g_reg_windowedmode && (_XCmdLineOption("xwindow",0) == 0) && (_XCmdLineOption("xmysetting",0) == 0) )
						{
							g_ChangeResolutionDrawLock = TRUE;

							gApp->Pause( true );
							::ShowWindow(wnd, SW_MINIMIZE);
							if( !_XChangeToFullScreen( g_PreviousResolution.cx, g_PreviousResolution.cy ) )
							{
								return 0;
							}
							gApp->Pause( false );
							gApp->m_bMinimized = true;
							gApp->RestoreGammaRamp();
						}
					}
				}
#endif
				
			}
			else
			{
				gApp->ReleaseClickCursor();

				/*
#ifdef _XTS_ITEMMALLBROWSER
				if( gApp->m_bMinimized )
				{
					gApp->Pause( true );
					if( !_XChangeToFullScreen( gnWidth, gnHeight ) )
					{
						return 0;
					}

					gApp->m_bMinimized = false;					
					gApp->RestorePrevResolution();

					::SetForegroundWindow( gHWnd );
					::ShowWindow(wnd, SW_SHOW);

					gApp->Pause( false );
					gApp->ApplyGammaRamp();
				}
#endif
				*/

				if( gApp )
				{
					if( g_UseHardwareCursor )
					{
						::SetCursor( NULL );
						if( gApp->m_bShowCursorWhenFullscreen )
						{
							if( gpDev )
								gpDev->ShowCursor( true );
						}
					}
					else
					{
						gApp->SetCursor( 0, TRUE );
					}
				}
			}
						
			break;

		case WM_SIZE:
			if(gApp) 
			{
				// Pick up possible changes to window style due to maximize, etc.
				if( gApp->m_bWindowed && gHWnd != NULL )
					gApp->m_dwWindowStyle = GetWindowLong( gHWnd, GWL_STYLE );
				
				if( SIZE_MINIMIZED == wparam )
				{					
					if( gApp->m_bClipCursorWhenFullscreen && !gApp->m_bWindowed ) ClipCursor( NULL );

#ifdef _XTS_ITEMMALLBROWSER
					
#else
					gApp->Pause( true ); // Pause while we're minimized
					gApp->m_bMinimized = true;
					gApp->m_bMaximized = false;
#endif
				}
				else if( SIZE_MAXIMIZED == wparam )
				{
					if( gApp->m_bMinimized )
						gApp->Pause( false ); // Unpause since we're no longer minimized

					gApp->m_bMinimized = false;
					gApp->m_bMaximized = true;
					gApp->HandlePossibleSizeChange();
				}
				else if( SIZE_RESTORED == wparam )
				{
#ifdef _XTS_ITEMMALLBROWSER
					if( g_LanguageType != _XLANGUAGE_TYPE_JAPANESE && g_LanguageType != _XLANGUAGE_TYPE_ENGLISH )
					{
						if( gApp->m_bMinimized )
						{
							gApp->Pause( true );
							if( !_XChangeToFullScreen( gnWidth, gnHeight ) )
							{
								return 0;
							}
							gApp->Pause( false );

							gApp->m_bMinimized = false;
							gApp->RestorePrevResolution();

							::ShowWindow(wnd, SW_SHOW);
							::SetForegroundWindow( gHWnd );
							::SetFocus( gHWnd );
							
							gApp->ApplyGammaRamp();

							gApp->m_bHasFocus = TRUE;
						}
					}
#else
					if( gApp->m_bMaximized )
					{
						gApp->m_bMaximized = false;
						gApp->HandlePossibleSizeChange();
					}
					else if( gApp->m_bMinimized)
					{
						gApp->Pause( false ); // Unpause since we're no longer minimized
						gApp->m_bMinimized = false;
						gApp->HandlePossibleSizeChange();
					}
					else
					{
						// If we're neither maximized nor minimized, the window size 
						// is changing by the user dragging the window edges.  In this 
						// case, we don't reset the device yet -- we wait until the 
						// user stops dragging, and a WM_EXITSIZEMOVE message comes.						
					}
#endif
				}
			}
            break;

		case WM_EXITSIZEMOVE:			
			if(gApp) 
			{
				gApp->Pause( false );
				gApp->HandlePossibleSizeChange();
			}			
            break;

		case WM_INITMENUPOPUP:
			//SwitchOut();
			break;
			
		case WM_MENUSELECT:
			if((HIWORD(wparam) == 0xFFFF) && (!lparam))
			{
				//SwitchIn();
			}
			break;

		case WM_ENTERMENULOOP:
            // Pause the app when menus are displayed
			if(gApp) 
			{
				gApp->Pause(true);
			}
            break;
			
        case WM_EXITMENULOOP:
			if(gApp) 
			{
				gApp->Pause(false);
			}
            break;

		case WM_NCHITTEST:
            // Prevent the user from selecting the menu in fullscreen mode
			if(gApp) 
			{
				if( !gApp->m_bWindowed ) return HTCLIENT;
			}
            break;
												
		//case WM_SETFOCUS:
		//	break;
		//case WM_KILLFOCUS:
		//	break;
			
		case WM_POWERBROADCAST:
			switch( wparam )
            {
			#ifndef PBT_APMQUERYSUSPEND
			#define PBT_APMQUERYSUSPEND 0x0000
			#endif
			case PBT_APMQUERYSUSPEND:
				// At this point, the app should save any data for open
				// network connections, files, etc., and prepare to go into
				// a suspended mode.
				return true;
				
			#ifndef PBT_APMRESUMESUSPEND
			#define PBT_APMRESUMESUSPEND 0x0007
			#endif
			case PBT_APMRESUMESUSPEND:
				// At this point, the app should recover any data, network
				// connections, files, etc., and resume running from when
				// the app was suspended.
				return true;
            }
			break;				
		case WM_SYSCOMMAND:							// Intercept System Commands
			if(gApp)
			{
				switch (wparam)						// Check System Calls
				{
                case SC_KEYMENU:
#ifdef _XDEF_KEYCHANGE_20070115
					SendMessage( wnd, WM_KEYDOWN, VK_F10, NULL );
					return true;
#endif
				case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					if( false == gApp->m_bWindowed ) return 1;
				}				
			}
			break;

		case MM_MCINOTIFY:
			if(gApp) gApp->MCINotify( message, wparam, lparam );
			break;

		case WM_CLOSE:
			{
				if(gApp) gApp->RestoreGammaRamp();	

#ifdef _XADMINISTRATORMODE
				if( g_hRenderStateControllerWindow )
					DestroyWindow(g_hRenderStateControllerWindow);
#endif
#ifdef _XDWDEBUG
				if( g_DebugWindowHandle )
					DestroyWindow(g_DebugWindowHandle);
#endif

				if( !g_NormalGameExit )
				{
					_XMsgClose = false;
					return 1;
				}
				else
				{				
					_XMsgClose = true;
					
					RECT rect;

					if( gApp->m_bWindowed )
					{				
						GetWindowRect( gHWnd, &rect );
						g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONX, rect.left );
						g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_WINDOWPOSITIONY, rect.top );
					}
				}

#ifdef _XKERNEL_KEYHOOK	
				{
					OSVERSIONINFO ver;	
					ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
					GetVersionEx(&ver);	
					if(ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
					{
						if(g_hKeyboardHook)
						{
							if( UnhookWindowsHookEx( g_hKeyboardHook ) == 0 )
							{
								_XLog( "ERROR : Can't release keyboard hooking" );
							}
							g_hKeyboardHook = NULL;
						}
					}
					else
					{
						DisableTaskSwitching();
					}
				}
#endif
			}
			break;
	}
 
	return DefWindowProc(wnd, message, wparam, lparam);
}

void FPSProc(void)
{
	gnFPS = gnFrameCounter;
	gnFrameCounter = 0;
}

void _XSetDefaultPath( LPSTR localpath  )
{
	memset( gPreviousPath, 0, _MAX_PATH );
	GetCurrentDirectory( _MAX_PATH-1, gPreviousPath );
	_chdir( gModulePath );
	if( localpath ) _chdir( localpath );	
}

void _XRestorePrevPath( void )
{
	_chdir( gPreviousPath );
}

void _XSetCaptureFileSubFolder( LPTSTR subpath )
{
	memset( gCaptureFileSubFolder, 0, sizeof(TCHAR) * _MAX_PATH );
	if( subpath && strlen(subpath) > 0 )
	{
		strcpy( gCaptureFileSubFolder, "./" );
		strcat( gCaptureFileSubFolder, subpath );
	}
}

BOOL CheckCalledLauncher( TCHAR* argbuf )
{
	int i, q;
	int nArgc;
	static TCHAR *pArgv[64]; 

	TCHAR tempbuf[2048];
	memset( tempbuf, 0, 2048 );
	strcpy( tempbuf, argbuf );
		
	nArgc = 0;
	i = 0;
	while(tempbuf[i])
	{
		while((tempbuf[i]) && (isspace(tempbuf[i])))
			i++;
		
		if(tempbuf[i])
		{
			if((tempbuf[i] == '\'') || (tempbuf[i] == '"'))
			{
				q = tempbuf[i++];
				if(!tempbuf[i])
					break;
			}
			else
				q = 0;
			
			pArgv[nArgc++] = &tempbuf[i];
			
			while((tempbuf[i]) && ((q) ? (tempbuf[i] != q) : (!isspace(tempbuf[i]))))
				i++;
			
			if(tempbuf[i])
			{
				tempbuf[i] = 0;
				i++;
			}
		}
	}	

	TCHAR buffer[256];
	memset( buffer, 0, 256 );	
	strncpy( buffer, _XDEF_LAUNCHER_SIGNAL1, 5 );
	strncpy( buffer+5, _XDEF_LAUNCHER_SIGNAL2, 5 );
	strncpy( buffer+10, _XDEF_LAUNCHER_SIGNAL3, 5 );
	strncpy( buffer+15, _XDEF_LAUNCHER_SIGNAL4, 5 );
	strncpy( buffer+20, _XDEF_LAUNCHER_SIGNAL5, 6 );
	
	_XEncryptor encrypt_factory;
	encrypt_factory.setKey( 10, 31, buffer );
	
	for( i = 0; i < nArgc; i++ )
	{
		if( encrypt_factory.checksumSerial( pArgv[i]+1 ) )
		{
			return TRUE;
		}
	}	

	return FALSE;
}

void _XExceptionClear( void )
{

#ifdef _XDEF_NEWENCRYPTION_PACKDATA
	// ..
#else
	_XSetDefaultPath();
	_XDeleteFile( g_XScriptDecryptFilename );
#endif

	if( g_XApplicationMutex )
		ReleaseMutex( g_XApplicationMutex );
}


int __stdcall WinMain(HINSTANCE hI, HINSTANCE hP, TCHAR *Cmd, int nShow)
{
	gHInst = hI;	

	// Set program start point...
	memset( gModulePath, 0, _MAX_PATH );
	GetCurrentDirectory( _MAX_PATH-1, gModulePath );
	
	memset( gPackTemporaryFilePath, 0, _MAX_PATH );
	::GetTempPath(_MAX_PATH, gPackTemporaryFilePath);
	DWORD curtime = timeGetTime();
	TCHAR tbuffer[32];
	sprintf( tbuffer, _T("t0x%d.tmp"), curtime );
	strcat( gPackTemporaryFilePath, tbuffer );
		
	g_XApplicationMutex = ::CreateMutex(NULL, TRUE, "9Dragons mutex object");	
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{	
		TCHAR filename[_MAX_PATH];
		time_t ltime;    
		struct tm *today;
		
		time( &ltime );
		today = localtime( &ltime );
								
		memset( filename, 0, sizeof(TCHAR) * _MAX_PATH );
		strftime( filename, 128, "%d%H%M%S", today );

		#ifdef _XDEF_NEWENCRYPTION_PACKDATA
			// ..		
		#else
			strcpy( g_XScriptDecryptFilename, _T("ndcore") );
			strcat( g_XScriptDecryptFilename, filename );
			strcat( g_XScriptDecryptFilename, _T(".dsc") );
		#endif
		
		TCHAR logfilename[64];
		strcpy( logfilename, _T("EOD") );
		strcat( logfilename, filename );
		strcat( logfilename, _T(".log") );

		_XSetLogFile(logfilename);

		ReleaseMutex( g_XApplicationMutex );
		g_XApplicationMutex = NULL;
	}
	else
	{
		if( g_XApplicationMutex )
		{
			_XSetLogFile("EOD.log");
			
			#ifdef _XDEF_NEWENCRYPTION_PACKDATA
				// ..
			#else
				strcpy( g_XScriptDecryptFilename, _T("ndcore.dsc") );
			#endif
		}
		else
		{
			_XSetLogFile("EOD.log");
			_XLog( "ERROR : Could not create app mutex object." );
			return 0x0;
		}
	}

	// Check called launcher...
#ifndef _XDWDEBUG
	if( !CheckCalledLauncher( Cmd ) )
	{
		_XExceptionClear();

		ShellExecute(NULL, NULL, _XDEF_LAUNCHER_FILENAME, NULL, NULL, SW_SHOWNORMAL);
		_XLog( "Call launcher." );
		return 0x0;
	}
#endif
			
	_XLog( "Initialize registry file." );
	g_RegManager.SetRegistryFolder( gModulePath );

	if( !CheckExistFile( "ndreg.xrg", FALSE ) )
	{
		_XLog( "Import window registry..." );
		g_RegManager.ImportWindowsRegistry();		
	}
	else
	{
		_XLog( "Load registry file..." );	
		g_RegManager.LoadRegistry();		
	}

#ifdef _XUSE_XTRAP
	TCHAR szGameName[_MAX_PATH] = {'\0',};
	DWORD apiVersion			= 0xA5001131;
	DWORD venderCode			= 0x00000012;
	DWORD keybordType			= 0x00000000;
	DWORD patchType				= 0x00000010;
	DWORD moduleType			= 0x0FFFFFFF;

	sprintf(szGameName, "%s", "NineDragons");

	SetXTrapPatchHttpUrl("http://patch.wiselogic.co.kr:10080/xtrap");

	SetXTrapStartInfo(szGameName, apiVersion, venderCode, keybordType, patchType, moduleType);

	XTrapStart();
	XTrapKeepAlive();
#endif

#ifdef _XUSE_HACKSHEILD
	g_dwMainThreadID = GetCurrentThreadId();	

	TCHAR szFullFileName[_MAX_PATH];
	memset( szFullFileName, 0, sizeof(TCHAR) * _MAX_PATH );
	strcpy( szFullFileName, gModulePath );
	lstrcat(szFullFileName, _T("\\HackShield\\EhSvc.dll"));

	DWORD dwOption = AHNHS_CHKOPT_ALL | AHNHS_USE_LOG_FILE;
	
#if defined(_XDWDEBUG) || defined(_XADMINISTRATORMODE) || defined(_DEBUG)
	dwOption &= ~AHNHS_CHKOPT_READWRITEPROCESSMEMORY;
	dwOption &= ~AHNHS_CHKOPT_KDTARCER;
#endif

	dwOption &= ~AHNHS_CHKOPT_OPENPROCESS;

	int nRet = 0;

#ifdef _DEBUG	// debug mode
	nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 1802, "35924E013581A126", dwOption); 
#else 
	#ifdef _XDWDEBUG // release debug mode
	nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 1803, "6F4D674342BA7CA2", dwOption);
	#else

		#ifdef _XADMINISTRATORMODE // admin mode
			nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 1804, "AA9419967E26A8BD", dwOption);
		#else  // release mode
			nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 1801, "8493F50441FA20F1", dwOption);
		#endif // admin mode
	#endif // release debug mode
#endif // debug mode
	
	if( nRet == HS_ERR_INVALID_PARAM )
	{
		_XExceptionClear();
		_XFatalError( "Hack shield initialize <Invalid parameter>" );
		_AhnHS_Uninitialize();
		return 0x0;
	}
	if( nRet == HS_ERR_INVALID_LICENSE )
	{
		_XExceptionClear();
		_XFatalError( "Hack shield initialize <Invalid licence>" );
		_AhnHS_Uninitialize();
		return 0x0;
	}
	if( nRet == HS_ERR_ALREADY_INITIALIZED )
	{
		_XExceptionClear();
		_XFatalError( "Hack shield initialize <Already initialized>" );
		_AhnHS_Uninitialize();
		return 0x0;
	}

	if (nRet != HS_ERR_OK) 
	{
		//Error 처리 
		switch(nRet)
		{
		case HS_ERR_ANOTHER_SERVICE_RUNNING:
			{
				MessageBox(NULL, _T("구룡쟁패가 실행중입니다.\n잠시 후에 재시도 하시기 바랍니다."), _T("XKernel"), MB_OK);
				break;
			}
		case HS_ERR_INVALID_FILES:
			{
				MessageBox(NULL, _T("잘못된 파일이 설치되었습니다.\n프로그램을 재설치 하시기 바랍니다."), _T("XKernel"), MB_OK);
				break;
			}
		case HS_ERR_SOFTICE_DETECT:
			{
				MessageBox(NULL, _T("컴퓨터에서 SoftICE 실행이 감지되었습니다.\nSoftICE 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다."), _T("XKernel"), MB_OK);
				break;
			}
		case HS_ERR_INIT_DRV_FAILED:
			{
				TCHAR szMsg[255];
				wsprintf(szMsg, _T("해킹방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다."), nRet);
				MessageBox(NULL, szMsg, _T("XKernel"), MB_OK);
				break;
			}
		}
		_XExceptionClear();
		_AhnHS_Uninitialize();
		return 0x0;
	}
	
	nRet = _AhnHS_StartService();
	assert(nRet != HS_ERR_NOT_INITIALIZED);
	assert(nRet != HS_ERR_ALREADY_SERVICE_RUNNING);
	
	if (nRet != HS_ERR_OK)
	{
		// nRet ==  HS_ERR_START_ENGINE_FAILED || nRet == HS_ERR_DRV_FILE_CREATE_FAILED
		// || nRet == HS_ERR_REG_DRV_FILE_FAILED || nRet == HS_ERR_START_DRV_FAILED
		_XExceptionClear();				
		TCHAR szMsg[255];
		wsprintf(szMsg, _T("해킹 방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다."), nRet);
		MessageBox(NULL, szMsg, _T("XKernel"), MB_OK);
		return 0x0;
	}
	
#endif
	
	memset( gCaptureFileSubFolder, 0, _MAX_PATH );

	_XLog( "Set process priority." );
	::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_HIGHEST );

#define _XDEF_USEEXCETIONHANDLER
	
#ifdef _XDEF_USEEXCETIONHANDLER
	__try
	{
#endif
		ConvertCommandLineToArgcArgv(GetCommandLine());
		_XLog( "Begin kernel." );

		// 일본은 gameguard basic license라서 실행파일 압축 사용 안함.
		#if !defined(_XDWDEBUG) && !defined(_XADMINISTRATORMODE) && !defined(_DEBUG) && !defined(_XGMCLIENT) && !defined(_XPLAYER_NO_LEGACY_ANTICHEAT) && !defined(_XJAPANESE) && !defined(_XRUSSIAN) && !defined(_XVIETNAMESE)

		LPCTSTR pModuleFilename = _XArgv(0);
		if( pModuleFilename )
		{
			_XSetDefaultPath();
			FILE* pFile = fopen( pModuleFilename, _T("rb") );
			if( pFile )
			{
				LONG curptr = ftell( pFile );
				fseek( pFile , 0L, SEEK_END );
				LONG endptr = ftell( pFile );
				LONG filesize = endptr - curptr;
				fclose( pFile );

				if( filesize > 2500000 )
				{
					_XLog( "ERROR : %d,%d", 1, g_LocalSystemTime );
					return 0x0;
				}
			}
			else
			{
				_XLog( "ERROR : %d,%d", 2, g_LocalSystemTime );
				return 0x0;
			}
		}
		else
		{
			_XLog( "ERROR : %d,%d", 3, g_LocalSystemTime );
			return 0x0;
		}

		#endif

		XKernelMain();
#ifdef _XDEF_USEEXCETIONHANDLER
	}
	__except( _XRecordExceptionInfo( GetExceptionInformation(), _T("TripleXKernel::XKernel.cpp") ) )
	{
		_XExceptionClear();
	}
#endif

#ifdef _XUSE_HACKSHEILD
	//서비스 종료 함수 호출 
	_AhnHS_StopService();
	//완료 함수 호출 
	_AhnHS_Uninitialize();
#endif	

	// ============================================
	if( g_XApplicationMutex )
	{
		ReleaseMutex( g_XApplicationMutex );
		_XLog( "Released 9dragons mutex." );
	}
	else
	{
		_XLog( "Released 9dragons instance object." );
	}

	return 0x0;
}


#ifdef _XADMINISTRATORMODE
void SetCommandConsoleWindow( void )
{
	if( !_XConsole ) return;
#ifdef _XDWDEBUG
	g_hRenderStateControllerWindow = CreateDialog(NULL, MAKEINTRESOURCE(IDD_COMMANDDIALOG), gHWnd, (DLGPROC)_XKernelCommandWindowProc);
#else	
	g_hRenderStateControllerWindow = CreateDialog(NULL, MAKEINTRESOURCE(IDD_ADMINCOMMANDDIALOG), gHWnd, (DLGPROC)_XKernelCommandWindowProc);	
#endif

	ShowWindow(g_hRenderStateControllerWindow, SW_SHOW); 
}
#endif

void SetDebugConsolWindow( void )
{
#ifdef _XDWDEBUG
	if( _XCmdLineOption("xdebug",0) != 0 || (_XCmdLineOption("xmysetting",0) != 0) )
	{
		_XDebugWindow = true;

		g_DebugWindowHandle = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DEBUGWINDOW), gHWnd, (DLGPROC)_XKernelDebugWindowProc); 
		ShowWindow(g_DebugWindowHandle, SW_SHOW); 		
		g_DebugWindowListHandle = GetDlgItem( g_DebugWindowHandle, IDC_DEBUGSTRINGLIST );
		
		RECT rect;
		GetWindowRect( g_DebugWindowHandle, &rect );
		int debugwinxpos = (GetSystemMetrics(SM_CXSCREEN)-(rect.right-rect.left))/2;
		int debugwinypos = (GetSystemMetrics(SM_CYSCREEN)-(rect.bottom-rect.top))/2;
		debugwinxpos = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWPOSITIONX, debugwinxpos );			
		debugwinypos = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWPOSITIONY, debugwinypos );

		int debugwinwidth  = rect.right-rect.left;
		int debugwinheight = rect.bottom-rect.top;

		debugwinwidth  = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWWIDTH, debugwinwidth );			
		debugwinheight = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWHEIGHT, debugwinheight );			

		MoveWindow( g_DebugWindowHandle, debugwinxpos, debugwinypos, debugwinwidth, debugwinheight, TRUE );

		::SetForegroundWindow( gHWnd );
	}
#endif

#ifdef _XADMINISTRATORMODE
	
	if( _XCmdLineOption("xconsole",0) != 0 || (_XCmdLineOption("xmysetting",0) != 0) )
	{
		_XConsole = true;
		
		WNDCLASS wc;
		GetClassInfo(NULL, "edit", &wc);
		g_lpfnBaseEditWndProc = (WNDPROC) wc.lpfnWndProc;
		wc.lpfnWndProc = _XKernelCommandEditWndProc;
		wc.hInstance = (HINSTANCE) GetWindowLong(gHWnd, GWL_HINSTANCE);
		wc.lpszClassName = "xcommandedit";
		
		if (RegisterClass((LPWNDCLASS) &wc) == false) 
		{
			return;
		}	
		
		SetCommandConsoleWindow();
	}
#endif

}

#ifdef _XADMINISTRATORMODE

void _XInsertConsoleWindowCommandHistory( LPSTR commandstring )
{
	if( _XConsole && g_hRenderStateControllerWindow )
	{
		HWND hconsolewindowhistorylist = GetDlgItem( g_hRenderStateControllerWindow, IDC_COMMANDHISTORYLIST );
		if( !hconsolewindowhistorylist )
		{
			_XLog( "ERROR : Can't find command history list control" );
			return;
		}

		int count = SendMessage( hconsolewindowhistorylist, LB_GETCOUNT, 0, 0 );
		TCHAR lbtext[512];
		int i = 0 ; for(  i = 0; i < count; i++ )
		{
			memset( lbtext, 0, sizeof(TCHAR)*512 );			
			if( count > i )
			{						
				SendMessage( hconsolewindowhistorylist, LB_GETTEXT, (WPARAM)i, (LPARAM)lbtext );
				if( strcmpi( lbtext, commandstring ) == 0 )
				{
					return;
				}
			}
		}

		
		int col = SendMessage( hconsolewindowhistorylist, LB_ADDSTRING, 0, (LPARAM)commandstring );		
		if( col > 50 )
		{
			SendMessage( hconsolewindowhistorylist, LB_DELETESTRING, 0, 0 );
			SendMessage( hconsolewindowhistorylist, LB_SETCURSEL, col-1, 0 );
		}
		else
		{
			SendMessage( hconsolewindowhistorylist, LB_SETCURSEL, col, 0 );
		}	
	}
}
#endif

#ifdef _XADMINISTRATORMODE
LRESULT CALLBACK _XKernelCommandEditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{	
	case WM_CHAR :
		switch(wParam) 
		{
		case VK_ESCAPE:			
			return 0;
			break;
		case VK_TAB:			
			return 0;
			break;
		case VK_RETURN:			
			return 0;
			break;
		}			
		break;
	case WM_KEYDOWN :		
		if (wParam == VK_RETURN) 
		{
			TCHAR commandstring[512];
			memset( commandstring, 0, 512 );
			GetWindowText( g_hRenderStateControllerEditWindow, commandstring, 511 );
			SetWindowText(g_hRenderStateControllerEditWindow, "");

			if( strlen( commandstring ) > 0 )
			{
				memset( g_LastCommandString, 0, 512 );
				strcpy( g_LastCommandString, commandstring );
				_XKernelCommandProcedure( g_LastCommandString );
			}			
			return 0;
		}
		else if (wParam == VK_TAB) 
		{			
			SetWindowText(hWnd, g_LastCommandString);
			SendMessage( hWnd, WM_KEYDOWN, VK_END, NULL );
			SendMessage( g_hRenderStateControllerEditWindow, EM_SETSEL, 0, strlen(g_LastCommandString) );
			return 0;
		}
		break;	
	}
	return CallWindowProc(g_lpfnBaseEditWndProc, hWnd, message, wParam, lParam); 
}

#endif

#ifdef _XADMINISTRATORMODE
void ShowRenderStateControlDlg( void )
{
	if( _XConsole && g_hRenderStateControllerWindow )
	{
		ShowWindow( g_hRenderStateControllerWindow, SW_SHOW );	
		SetFocus( g_hRenderStateControllerEditWindow );
		SetWindowText(g_hRenderStateControllerEditWindow, g_LastCommandString);
		SendMessage( g_hRenderStateControllerEditWindow, WM_KEYDOWN, VK_END, NULL );
		SendMessage( g_hRenderStateControllerEditWindow, EM_SETSEL, 0, strlen(g_LastCommandString) );
	}
}
#endif

#ifdef _XDWDEBUG
LRESULT CALLBACK _XKernelDebugWindowProc( HWND hDlg, UINT message, WPARAM wparam, LPARAM lparam  )
{  
  switch (message) 
  {    
	case WM_SIZE:
		{
			RECT rect;
			GetClientRect( hDlg, &rect );

			if( g_DebugWindowListHandle )
			{				
				MoveWindow( g_DebugWindowListHandle, 1, 1, rect.right-rect.left-2, rect.bottom-rect.top-22, TRUE );				
			}
			HWND clearbtnhandle = GetDlgItem( hDlg, IDC_CLEARBUTTON );
			if( clearbtnhandle )
			{
				RECT btnrect;
				GetClientRect( clearbtnhandle, &btnrect );
				MoveWindow( clearbtnhandle, rect.right-btnrect.right, rect.bottom-btnrect.bottom, btnrect.right, btnrect.bottom, TRUE );
			}
		}
		break;
    case WM_COMMAND:
		{        
			UINT wID = LOWORD(wparam);         // item, control, or accelerator identifier 
			HWND hwndCtl = (HWND) lparam;      // handle of control 
    
			switch(wID)
			{
			case IDC_CLEARBUTTON :
				{
					SendMessage( g_DebugWindowListHandle, LB_RESETCONTENT, 0, 0 );
				}
				break;
			default:
				break;
			} // switch wID
		} // WM_COMMAND block
      break;	
	case WM_DESTROY:
		{
			RECT rect;
			GetWindowRect( g_DebugWindowHandle, &rect );
			g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWPOSITIONX, rect.left );
			g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWPOSITIONY, rect.top );
			g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWWIDTH, rect.right - rect.left );
			g_RegManager._XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_DEBUGWINDOWHEIGHT, rect.bottom - rect.top );		
			
			g_DebugWindowListHandle = NULL;

			g_DebugWindowHandle = NULL;
		}
	  break;		
  } // switch uMsg 
  
  return FALSE;
}
#endif

BOOL SaveSurfaceToJPG( IDirect3DSurface9*& pSrcSurface, LPTSTR szFilename )
{
	if( !pSrcSurface ) return FALSE;

	D3DLOCKED_RECT data;
	D3DSURFACE_DESC sufdesc;
	IDirect3DSurface9* pDestSurface;
	pSrcSurface->GetDesc( &sufdesc );

	_XLog( "Capture : Create temporary surface." );
	HRESULT hr = gpDev->CreateOffscreenPlainSurface(sufdesc.Width, sufdesc.Height, sufdesc.Format, 
													D3DPOOL_SYSTEMMEM, &pDestSurface, NULL);
	if(FAILED(hr))
	{		
		_XDWINPRINT("WARNING : Can't create temporary surface.");
		return FALSE;		
	}			
	
	_XLog( "Capture : Get render target data." );
	hr = gpDev->GetRenderTargetData(pSrcSurface, pDestSurface);
	if(FAILED(hr))
	{		
		_XDWINPRINT("WARNING : Can't copied backbuffer.");
		return FALSE;
	}
	
	if( sufdesc.Format != D3DFMT_X8R8G8B8 && sufdesc.Format != D3DFMT_A8R8G8B8 )
	{
		IDirect3DSurface9* pNewDestSurface;

		HRESULT hr = gpDev->CreateOffscreenPlainSurface(sufdesc.Width, sufdesc.Height, D3DFMT_X8R8G8B8,
														D3DPOOL_SYSTEMMEM, &pNewDestSurface, NULL);
		if(FAILED(hr))
		{		
			_XDWINPRINT("WARNING : Can't create secondary temp.. surface.");
			return FALSE;		
		}

		hr = D3DXLoadSurfaceFromSurface( pNewDestSurface, NULL, NULL, pDestSurface, NULL, NULL, D3DX_FILTER_NONE, 0 );

		if(FAILED(hr))
		{		
			_XDWINPRINT("WARNING : Can't copy convert surface.");
			return FALSE;		
		}

		SAFE_RELEASE( pDestSurface );
		pDestSurface = pNewDestSurface;		
	}

	_XLog( "Capture : Locking render target data." );
	hr = pDestSurface->LockRect(&data, NULL, 0);
	if(FAILED(hr))
	{			
		SAFE_RELEASE( pDestSurface );
		_XDWINPRINT("WARNING : Surface lock failed");
		return FALSE;
	}
	
	_XLog( "Capture : Start jpeg compressing and saving" );
	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;	
	FILE* outfile=NULL;			/* target file */
	
	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
		
	/* Now we can initialize the JPEG compression object. */
	if (setjmp(jerr.setjmp_buffer)) {
	/* If we get here, the JPEG code has signaled an error.
	* We need to clean up the JPEG object, close the input file, and return.
		*/
		
		jpeg_destroy_compress(&cinfo);
		
		pDestSurface->UnlockRect();
		pDestSurface->Release();
		
		if (outfile!=NULL) fclose(outfile);
		
		return FALSE;
	}

	_XLog( "Capture : jpeg_create_compress" );
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	if ((outfile = fopen(szFilename, "wb")) == NULL) 
	{		
		_XDWINPRINT("WARNING : Jpeg file : can't open %s", szFilename);		
		// Unlock the surface
		pDestSurface->UnlockRect();
		pDestSurface->Release();
		return FALSE;
	}

	_XLog( "Capture : jpeg_stdio_dest" );
	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */
												    
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = sufdesc.Width; 	/* image widthPix and height, in pixels */
	cinfo.image_height = sufdesc.Height;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	 
/* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */

 _XLog( "Capture : jpeg_set_defaults" );
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  int ScreenCaptureQuality = g_RegManager._XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_CAPTUREQUALITY, 75, TRUE );
  if( ScreenCaptureQuality > 100 ) ScreenCaptureQuality = 100;
  if( ScreenCaptureQuality < 30  ) ScreenCaptureQuality = 30;

  _XLog( "Capture : jpeg_set_quality" );
  jpeg_set_quality(&cinfo, ScreenCaptureQuality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  _XLog( "Capture : jpeg_start_compress" );
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */  
  _XLog( "Capture : copy capture data" );
  LPBYTE pBitmap = (LPBYTE)data.pBits;
  LPBYTE ScanLineBuffer = new BYTE[data.Pitch];

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */

	 //LPBYTE outRow;	
	 //outRow = pBitmap + (cinfo.next_scanline * sufdesc.Width * 3);

	 int i = 0 ; for(  i = 0 ; i < sufdesc.Width; i++ )
	 {
		ScanLineBuffer[i*3  ] = pBitmap[(i<<2)+2];
		ScanLineBuffer[i*3+1] = pBitmap[(i<<2)+1];
		ScanLineBuffer[i*3+2] = pBitmap[(i<<2)+0];
	 }	

	 (void) jpeg_write_scanlines(&cinfo, &ScanLineBuffer, 1);

	 pBitmap += data.Pitch;
  }

  SAFE_DELETE_ARRAY( ScanLineBuffer )  

  // Unlock the surface
  pDestSurface->UnlockRect();
  pDestSurface->Release();

  _XLog( "Capture : Unlocking render target data." );

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);

  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);
  
  /* And we're done! */

  TCHAR message[256];
  if( g_LanguageType == _XLANGUAGE_TYPE_KOREAN )
	sprintf( message, "스크린 샷 %s 저장.", szFilename );
  else
	sprintf( message, "Saved %s", szFilename );
  
  _XInsertChatString( message, 11 );

  return TRUE;
}

/*
HRESULT RenderTiled(LPCSTR fileName,  int numTiles, HRESULT renderFunc() )
{
    HRESULT hr;
	
    // get the backbuffer description
    LPDIRECT3DSURFACE9 backbuf;
    if(FAILED(hr=gpDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backbuf))) 
        return hr;

    D3DSURFACE_DESC desc;
    hr=backbuf->GetDesc(&desc);
    backbuf->Release();

    if(FAILED(hr))
        return hr;
	
    // calculate final image size
    int imageWidth = desc.Width*numTiles;
    int imageHeight= desc.Height*numTiles;
	
    // create the image buffer
    LPDIRECT3DSURFACE9 surf;
    if (FAILED(hr=gpDev->CreateImageSurface(imageWidth, imageHeight, desc.Format, &surf))) 
        return hr;
	
    // get the current projection matrix and save a copy
    D3DXMATRIX oldProj,newProj;
    gpDev->GetTransform(D3DTS_PROJECTION,&oldProj);
    newProj=oldProj;
	
    // scale the projection matrix on x and y axis
    newProj._11*=numTiles;
    newProj._22*=numTiles;
	
    // loop through the tiles in X
    int i = 0 ; for ( i=0;i<numTiles;i++) 
	{		
        // offset x coordinates
        newProj._31=(numTiles-1)-i*2.0f;
		
        // loop through the tiles in Y
        for (int j=0;j<numTiles;j++) {
			
            // offset y coordinates
            newProj._32=-((numTiles-1)-j*2.0f);
			
            // set the modified projection matrix
            gpDev->SetTransform(D3DTS_PROJECTION,&newProj);
			
            // call the rendering function
            if (FAILED(hr=renderFunc())) {
                surf->Release();
                return hr;
            }
			
            // get the back buffer pointer
            if(FAILED(hr=gpDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backbuf))) {
                surf->Release();
                return hr;
            }
			
            // copy the tile to the image buffer and release the back buffer
            RECT destRect;
            destRect.left=desc.Width*i;
            destRect.right=destRect.left+desc.Width;
            destRect.top=desc.Height*j;
            destRect.bottom=destRect.top+desc.Height;
            hr=D3DXLoadSurfaceFromSurface(surf,NULL,&destRect, backbuf,NULL,NULL,D3DX_FILTER_NONE,0);
            backbuf->Release();
            if (FAILED(hr)) {
                surf->Release();
                return hr;
            }
			
            // show current tile
            gpDev->Present(0,0,0,0);
        }
    }
	
    // restore projection matrix
    gpDev->SetTransform(D3DTS_PROJECTION,&oldProj);
	
    // save the image to specified file
    hr=D3DXSaveSurfaceToFile(fileName,D3DXIFF_BMP,surf,NULL,NULL);
    surf->Release();
	
    // return status of save to caller
    return hr;
} 

*/