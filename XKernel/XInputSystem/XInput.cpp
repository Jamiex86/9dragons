#include "stdafx.h"

#include "XInput.h"
#include "XError.h"

void _XInput::pShutdownKb()
{
	if(m_lpDIKeyboard)
	{
		m_lpDIKeyboard->Unacquire();
		m_lpDIKeyboard->Release();
		m_lpDIKeyboard = NULL;
	}
}

void _XInput::pShutdownMouse()
{
#ifdef _XDEF_USEDIRECTINPUT	
	if(m_lpDIMouse)
	{
		m_lpDIMouse->Unacquire();
		m_lpDIMouse->Release();
		m_lpDIMouse = NULL;
	}
#endif
}

_XInput::_XInput(HWND hwnd,			// windows handle of the app window 
			   HINSTANCE hInst,		// Handle to the Instance of the Window
			   BOOL bExclusive		// Bool to set whether the mouse and keboard 
									// used exclusively by the app
			   )
{
	HRESULT hr;
	
	// initailize all variable
	m_bExclusive	= bExclusive;
	m_hwnd			= hwnd;
	m_bMouse		= FALSE;
	m_bKeyboard		= FALSE;
	ZeroMemory(m_KbBuffer, KBBUFFER_SIZE);	
	ZeroMemory(m_byKeyTemp, sizeof(BYTE)*256);	//Author : 양희왕
	m_ScrnPos.x		= 0;
	m_ScrnPos.y		= 0;
	m_ScrnPos.z		= 0;
	m_lMouseRes		= 1000;
	m_DIDLL			= NULL;
	m_lpDIMouse		= NULL;
	m_lpDIKeyboard	= NULL;
	
/*	// Load the DirectInput DLL
	m_DIDLL = LoadLibrary("DINPUT8.DLL");
	if(!m_DIDLL)
		_XFatalError("DINPUT8.DLL 로딩에 실패했습니다 !");

	// Get the Direct3DCreate8 function from the DLL
	m_dllDirectInput8Create = (HRESULT(WINAPI*)(HINSTANCE,DWORD,REFIID,LPVOID*,LPUNKNOWN))GetProcAddress(m_DIDLL, "DirectInput8Create");
	if(!m_dllDirectInput8Create)
		_XFatalError("DINPUT8.DLL에서 0x00F012E0 모듈을 찾을 수 없습니다 !");

	//Creating the DirectInput Object
	hr = m_dllDirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_lpDI, NULL);
	if(FAILED(hr))
	{
		// handle error
		_XFatalError("DINPUT 인터페이스 생성에 실패했습니다 !");
		//	return FALSE;
	}	*/
	
	if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_lpDI, NULL ) ) )
	{
		_XFatalError("Could not create Direct Input 8 interface!");
	}
}

_XInput::~_XInput()
{
	if(m_lpDI)
	{
		pShutdownKb();

#ifdef _XDEF_USEDIRECTINPUT	
		pShutdownMouse();
#endif
		m_lpDI->Release();
		m_lpDI = NULL;
	}

	if(m_DIDLL)
	{
		FreeLibrary(m_DIDLL);
		m_DIDLL = NULL;
		m_dllDirectInput8Create = NULL;
	}
}

BOOL _XInput::CreateKeyboard()
{
	HRESULT hr;
	
	if(!m_lpDI)
	{
		_XLogError("_XInput::CreateKeyboard() : Could not fine Direct Input 8 Object!");
		return FALSE;
	}

	pShutdownKb(); // to be sure

	// Geting the DirectInput Device Interface
	hr = m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIKeyboard, NULL);	
	if(FAILED(hr))
	{
		// shut down DXINPUT
		_XLogError("_XInput::CreateKeyboard() : DINPUT / Could not create Keyboard interface!");
		return FALSE;
	}
	
	// set keyboard data format
	hr = m_lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);	
	if(FAILED(hr))
	{
		// shut down down DX
		_XLogError("_XInput::CreateKeyboard() : Could not install data format!");
		pShutdownKb();
		return FALSE;
	}
	
	// setup keyboard behavior
//	if(m_bExclusive == TRUE)
		//hr = m_lpDIKeyboard->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
//	else
		hr = m_lpDIKeyboard->SetCooperativeLevel(m_hwnd, DISCL_BACKGROUND  | DISCL_NONEXCLUSIVE);
	
	if(FAILED(hr))
	{
		// shut down down DX
		_XLogError("_XInput::CreateKeyboard() : Could not set cooperative level!");
		pShutdownKb();
		return FALSE;
	}
	
	// Aquire the device
	if(m_lpDIKeyboard) 
	{	
		if( DI_OK != m_lpDIKeyboard->Acquire() )
		{
			_XLogError("_XInput::CreateKeyboard() : Failed acquire !");
			pShutdownKb();
			return FALSE;
		}
	}

	m_bKeyboard = TRUE;
	return TRUE;
}

BOOL _XInput::CreateMouse(int width, int height)
{

#ifdef _XDEF_USEDIRECTINPUT	

	HRESULT hr;
	
	if(!m_lpDI)
	{
		_XLogError("_XInput::CreateMouse() : Could not find Direct Input 8 interface!");
		return FALSE;
	}

	pShutdownMouse(); // to be sure
	
	m_ScreenWidth = width;
	m_ScreenHeight = height;
	
	hr = m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIMouse, NULL);
	if( hr != DI_OK )
	{
		_XLogError("_XInput::CreateMouse() : DINPUT / Could not create mouse device!");
		// shutdown
		return FALSE;
	}
	
	hr = m_lpDIMouse->SetDataFormat(&c_dfDIMouse2);	
	if( hr != DI_OK )
	{
		_XLogError("_XInput::CreateMouse() : Could not install data format!");
		// shutdown
		pShutdownMouse();
		return FALSE;
	}

/*	if(m_bExclusive == TRUE)
		hr = m_lpDIMouse->SetCooperativeLevel(m_hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	else*/
		hr = m_lpDIMouse->SetCooperativeLevel(m_hwnd, DISCL_BACKGROUND  | DISCL_NONEXCLUSIVE);
	
	if( hr  != DI_OK )
	{
		_XLogError("_XInput::CreateMouse() - Could not set cooperative level!");
		// shutdown
		pShutdownMouse();
		return FALSE;
	}

	/*
	// set the axis data to Absolute 
	
	//....Set the DIPROP structure
    DIPROPDWORD dipdw;	  
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_ABS;
	
	//.... Set the Property
	hr = m_lpDIMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
	  
	if FAILED(hr)
	{
		MessageBox(NULL, TEXT("Failed set Axis data to absolute mode"),
		TEXT("Error"),MB_OK);
		// shutdown
		return FALSE;
	}
	*/
/*	::SetFocus( gHWnd );
	hr = m_lpDIMouse->Acquire();
	if( hr != DI_OK )
	{
		_XLogError("_XInput::CreateMouse() : 마우스 장치를 얻어오는데 실패했습니다 !");
		// shutdown
		pShutdownMouse();
		return FALSE;
	}*/

	if( m_lpDIMouse )
		m_lpDIMouse->Acquire();
	
	m_bMouse = TRUE;

	POINT p;
	p.x = m_ScreenWidth>>1;
	p.y = m_ScreenHeight>>1;
	ClientToScreen(m_hwnd, &p);
	SetCursorPos(p.x, p.y);

	::SetFocus( gHWnd );

#endif

	return TRUE;
}

BOOL _XInput::CreateJoystick(long lJoyIndex)
{
	return TRUE;
}

void _XInput::Update()
{
	HRESULT hr;

#ifdef _XDEF_USEDIRECTINPUT	

	DIMOUSESTATE2 MouseState2;
	ZeroMemory( &MouseState2, sizeof(DIMOUSESTATE2) );
	// Update Mouse if it is created
	if(m_bMouse == TRUE)
	{
		//....Get MouseState and Fill up the MouseState Structure		
		hr = m_lpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2),(LPVOID)&MouseState2);
		if(FAILED(hr))
		{
			hr = m_lpDIMouse->Acquire();

			// if failed check if input is lost and require if lost
			while( hr == DIERR_INPUTLOST ) 
					hr = m_lpDIMouse->Acquire();
			
		}
		
		// fill up the MouseAction structure
		m_MouseState.AX = MouseState2.lX;
		m_MouseState.AY = MouseState2.lY;
		m_MouseState.AZ = MouseState2.lZ;		
		if(MouseState2.rgbButtons[0] & 0x80)
			m_MouseState.bButton[0] = true;
		else
			m_MouseState.bButton[0] = false;
		
		if(MouseState2.rgbButtons[1] & 0x80)
			m_MouseState.bButton[1] = true;
		else
			m_MouseState.bButton[1] = false;
		
		if(MouseState2.rgbButtons[2] & 0x80)
			m_MouseState.bButton[2] = true;
		else
			m_MouseState.bButton[2] = false;
		
		//....Update the Mouse Screen position
/*		m_ScrnPos.x += m_MouseState.AX;
		m_ScrnPos.y += m_MouseState.AY;
		m_ScrnPos.z += m_MouseState.AZ;
		
		if(m_ScrnPos.x > m_lMouseRes)
			m_ScrnPos.x = m_lMouseRes;
		if(m_ScrnPos.y > m_lMouseRes)
			m_ScrnPos.y = m_lMouseRes;
		if(m_ScrnPos.z > m_lMouseRes)
			m_ScrnPos.z = m_lMouseRes;
		
		if(m_ScrnPos.x < 0)
			m_ScrnPos.x = 0;
		if(m_ScrnPos.y < 0)
			m_ScrnPos.y = 0;
		if(m_ScrnPos.z < 0)
			m_ScrnPos.z = 0;		*/

		/*
		POINT p;
		p.x = m_ScreenWidth>>1;
		p.y = m_ScreenHeight>>1;
		ClientToScreen(m_hwnd, &p);
		SetCursorPos(p.x, p.y);
		*/ // Reset mouse position... 2002/ 6/26
	}

#endif
	// Update KeyBoard Buffer if Keyboard is created
	// ....Clear the keyboard buffer

	if( m_bKeyboard )
	{
		memset( m_KbBuffer, 0, sizeof(unsigned char) * 256 );
		hr = m_lpDIKeyboard->GetDeviceState( 256, (LPVOID)&m_KbBuffer);	
		if(FAILED(hr))
		{
			// if failed check if input is lost and require if lost
			if(hr == (DIERR_INPUTLOST | DIERR_NOTACQUIRED)  )
				m_lpDIKeyboard->Acquire();
			
			hr = m_lpDIKeyboard->GetDeviceState( 256, (LPVOID)&m_KbBuffer);
		}
	}

	// Update Joystick 
}


//Author : 양희왕
//breif : 구조 바꾼뒤 삭제
BOOL _XInput::GetKeyboardState(DWORD& dwKey)
{
	memset( m_KbBuffer, 0, sizeof(unsigned char) * 256 );
	HRESULT hr = m_lpDIKeyboard->GetDeviceState( 256, (LPVOID)&m_KbBuffer);	
	if(FAILED(hr))
	{
		// if failed check if input is lost and require if lost
		if(hr == (DIERR_INPUTLOST | DIERR_NOTACQUIRED)  )
			m_lpDIKeyboard->Acquire();
		
		hr = m_lpDIKeyboard->GetDeviceState( 256, (LPVOID)&m_KbBuffer);
	}

	if(hr == DI_OK)
	{	
		if(m_KbBuffer[dwKey] & 0x80)	// 초기화 후 키가 눌린 상태면
		{
			if(!(m_byKeyTemp[dwKey] & 0x80)) // 템프 키가 안눌려 있으면
			{
				m_byKeyTemp[dwKey] |= 0x80; // 템프키 누른 상태 추가
				m_KbBuffer[dwKey]	 |= 0x40; // 눌린 키 0x40 추가하여 0x40 검사로 한번만 클릭 체크
			}
			// 두번째 프레임 때 초기화 후 키가 눌린 상태로 들어오면 템프키는 눌러져 있는 상태 이므로 0x40 이 추가 안됨 
			// 그레서 0x40 으로 검사할 땐 통과 함 ...그래서 누르고 있어도 한번만 체크 
		}
		else
		{
			if(m_byKeyTemp[dwKey] & 0x80) //눌린상태 아니고 템프키는 눌려 있다
			{
				m_byKeyTemp[dwKey] &= 0x7f; // 0x80 = 1000 0000, 0x7f = 0111 1111 그래서... & 하면 0000 0000 으로초기화..
				m_KbBuffer[dwKey]	 |= 0x20; // 0x20 추가해서 0x20 으로 띈거 체크...
			}
		}

		return TRUE; //하나의 키라도 눌린 상황
	}

	return FALSE; // 어떤키도 눌린 상태가 아님
}

BOOL _XInput::CheckKeyPress(DWORD Key)
{
	if(m_KbBuffer[Key] & 0x80)
	{
		return TRUE;
	}

	return FALSE;
}

//Author : 양희왕 //사용안함 
BOOL _XInput::CheckKeyDown(DWORD Key)
{
	if(m_KbBuffer[Key] & 0x40)
	{
		return TRUE;
	}
	
	return FALSE;	
}

MouseState* _XInput::GetMouseState()
{
	return &m_MouseState;
}

ScrnPos* _XInput::GetMouseScreenPos()
{
	static ScrnPos ScreenPosition;	// if not static it return the address of a local variable...
	
	ScreenPosition.x = (m_ScrnPos.x/(float)m_lMouseRes)*m_ScreenWidth;
	ScreenPosition.y = (m_ScrnPos.y/(float)m_lMouseRes)*m_ScreenHeight;
	
	return &ScreenPosition;
}

ScrnPos* _XInput::GetMouseVirScreenPos()
{
	return &m_ScrnPos;
}

ScrnPos* _XInput::GetMousePrevScreenPos()
{
	return &m_PrevScrnPos;
}

void _XInput::SetScrnResolution(long width, long height)
{
	m_ScreenHeight	= height;
	m_ScreenWidth	= width; 
}

void _XInput::SetMouseResolution(long iMouseRes)
{
	m_lMouseRes = iMouseRes;
}

BOOL _XInput::CheckMousePos( int x1, int y1, int x2, int y2 )
{
	return(  x1 < m_ScrnPos.x && x2 > m_ScrnPos.x && y1 < m_ScrnPos.z && y2 > m_ScrnPos.z );
}