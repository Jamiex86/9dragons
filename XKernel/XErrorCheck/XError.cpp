#include "stdafx.h"

#include "xerror.h"
#include "x3dapp.h"

static TCHAR strDefaultBuffer[1024];

TCHAR *GetD3DErrorString(HRESULT hr, char *buf)
{
	if(!buf)
		buf = strDefaultBuffer;
	
#define ERRORCASE(error) case error: strncpy(buf, #error, MAX_CHAR_STRING); break
	
	switch(hr)
	{
		ERRORCASE(D3DERR_WRONGTEXTUREFORMAT);
		ERRORCASE(D3DERR_UNSUPPORTEDCOLOROPERATION);
		ERRORCASE(D3DERR_UNSUPPORTEDCOLORARG);
		ERRORCASE(D3DERR_UNSUPPORTEDALPHAOPERATION);
		ERRORCASE(D3DERR_UNSUPPORTEDALPHAARG);
		ERRORCASE(D3DERR_TOOMANYOPERATIONS);
		ERRORCASE(D3DERR_CONFLICTINGTEXTUREFILTER);
		ERRORCASE(D3DERR_UNSUPPORTEDFACTORVALUE);
		ERRORCASE(D3DERR_CONFLICTINGRENDERSTATE);
		ERRORCASE(D3DERR_UNSUPPORTEDTEXTUREFILTER);
		ERRORCASE(D3DERR_CONFLICTINGTEXTUREPALETTE);
		ERRORCASE(D3DERR_DRIVERINTERNALERROR);
		ERRORCASE(D3DERR_NOTFOUND);
		ERRORCASE(D3DERR_MOREDATA);
		ERRORCASE(D3DERR_DEVICELOST);
		ERRORCASE(D3DERR_DEVICENOTRESET);
		ERRORCASE(D3DERR_NOTAVAILABLE);
		ERRORCASE(D3DERR_OUTOFVIDEOMEMORY);
		ERRORCASE(D3DERR_INVALIDDEVICE);
		ERRORCASE(D3DERR_INVALIDCALL);
		ERRORCASE(D3DERR_DRIVERINVALIDCALL);
	default: strncpy(buf, "D3DERR_UNKNOWN", MAX_CHAR_STRING); break;
	}
	
#undef ERRORCASE
	
	return buf;
}

extern bool _XMsgClose;
extern BOOL g_NormalGameExit;
extern class X3DApp* gApp;

void _XFatalError(TCHAR *str, ...)
{
	TCHAR strVBuffer[1024];
	va_list ap;
	va_start(ap, str);
	vsprintf(strVBuffer, str, ap);
	va_end(ap);

	_XLogError("%s", strVBuffer);
	
	//assert(!"error");
	
	MessageBox(gHWnd, strVBuffer, "XKernel error", MB_OK | MB_ICONERROR);
	
	if(gApp) gApp->Shutdown();

/*	
#ifdef _XDWDEBUG
	if( _XDebugWindow  )
	{
		if (!FreeConsole())
			_XMessage("Could not free the console!");
	}
#endif			

	exit(0x1);
*/
	g_NormalGameExit = TRUE;
	_XMsgClose = TRUE;	

	//PostMessage( gHWnd, WM_CLOSE, 0 , 0 );
}

