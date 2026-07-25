#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include "xdialogs.h"

// Hold the path of the initial directory open
static XString strInitialDir(".\\");

static UINT APIENTRY OFNHook(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(uiMsg == WM_INITDIALOG)
	{
		hdlg = GetParent(hdlg);
		RECT rcOwner,rcDlg;
		
		GetWindowRect(GetParent(hdlg),&rcOwner);
		GetWindowRect(hdlg,&rcDlg);
		int x = (rcOwner.right+rcOwner.left-rcDlg.right+rcDlg.left)/2;
		int y = (rcOwner.bottom+rcOwner.top-rcDlg.bottom+rcDlg.top)/2;
		POINT p;
		p.x = p.y = 0;
		ClientToScreen(gHWnd, &p);
		if(y < p.y) y = p.y;
		SetWindowPos(hdlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

		return 1;
	}
	return 0;
}

void SetDialogsInitialDir(TCHAR* p_strInitialDir)
{
	strInitialDir = p_strInitialDir;

	TCHAR *s = strInitialDir;
	while(*s)
	{
		if(*s == '/')
			*s = '\\';
		s++;
	}

	if(*(s-1) != '\\')
		strInitialDir += "\\";
}


bool OpenFileDialog(XString& p_strFilePath, TCHAR* p_strTitle, TCHAR* p_strFilters,
				    TCHAR* p_strDefExt, bool p_bChangeDir)
{
	OPENFILENAME of;
	bool ret = true;

	
	TCHAR strTmp[_MAX_PATH];
	memset(strTmp, 0, _MAX_PATH);

	memset(&of,0,sizeof(of));
	of.lStructSize = sizeof(of);
	of.hwndOwner = NULL;
	of.hInstance = 0;
	of.lpstrFilter = p_strFilters;
	of.nFilterIndex = 1;
	of.lpstrFile = strTmp;
	of.lpstrInitialDir = strInitialDir;
	of.lpstrTitle = p_strTitle;
	of.nMaxFile = _MAX_PATH-1;
	of.nFileOffset = 0;
	of.nFileExtension = 0;
	of.lpstrDefExt = p_strDefExt;
	of.lpfnHook = OFNHook;
	of.Flags = OFN_FILEMUSTEXIST | OFN_ENABLEHOOK;
	if(!p_bChangeDir)
		of.Flags |= OFN_NOCHANGEDIR;

	RECT rect;
	GetWindowRect(gHWnd, &rect);
	if(gHWnd && (rect.left > 0 || rect.right > 0 || rect.top > 0 || rect.bottom > 0))
	{
		of.hwndOwner = gHWnd;
		of.hInstance = gHInst;
		of.Flags |= OFN_EXPLORER;
		_XLog("R: %d, %d, %d, %d", rect.left, rect.right, rect.top, rect.bottom);
	}
	
	ret = GetOpenFileName(&of)?true:false;
	
	if(ret == true)
		p_strFilePath = strTmp;
	
	return ret;
}

bool SaveFileDialog(XString& p_strFilePath, TCHAR* p_strTitle, TCHAR* p_strFilters,
				    TCHAR* p_strDefExt, bool p_bChangeDir)
{
	OPENFILENAME of;
	bool ret = true;
	
	TCHAR strTmp[_MAX_PATH];
	memset(strTmp, 0, _MAX_PATH);
	
	memset(&of,0,sizeof(of));
	of.lStructSize = sizeof(of);
	of.hwndOwner = NULL;
	of.hInstance = 0;
	of.lpstrFilter = p_strFilters;
	of.nFilterIndex = 1;
	of.lpstrFile = strTmp;
	of.lpstrTitle = p_strTitle;
	of.lpstrInitialDir = strInitialDir;
	of.nMaxFile = _MAX_PATH-1;
	of.nFileOffset = 0;
	of.nFileExtension = 0;
	of.lpstrDefExt = p_strDefExt;
	of.lpfnHook = OFNHook;
	of.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT |
			   OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	if(!p_bChangeDir)
		of.Flags |= OFN_NOCHANGEDIR;

	RECT rect;
	GetWindowRect(gHWnd, &rect);
	if(gHWnd && (rect.left > 0 || rect.right > 0 || rect.top > 0 || rect.bottom > 0))
	{
		of.hwndOwner = gHWnd;
		of.hInstance = gHInst;
		of.Flags |= OFN_EXPLORER;
	}

	ret = GetSaveFileName(&of)?true:false;
	
	if(ret == true)
		p_strFilePath = strTmp;
	
	return ret;
}

bool ChooseColorDialog(_XRGB *out)
{
	static COLORREF cr[16];
	CHOOSECOLOR cc;
	bool ret = true;
	
	memset(&cc,0,sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = gHWnd;
	cc.hInstance = 0;
	cc.rgbResult = RGB(out->r,out->g,out->b);
	cc.lpCustColors = cr;
	cc.Flags = CC_RGBINIT|CC_FULLOPEN;
	
	ret = ChooseColor(&cc)?true:false;

	if(ret == true)		
	{
		out->r = GetRValue(cc.rgbResult);
		out->g = GetGValue(cc.rgbResult);
		out->b = GetBValue(cc.rgbResult);
	}

	return ret;
}

