// XFileTransferDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XFileTransfer.h"
#include "XFileTransferDlg.h"
#include "Registry.h"
#include "RegistryDef.h"
#include "xzip.h"
#include <tchar.h>
#include <direct.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

static TCHAR * lstrrchr(LPCTSTR string, int ch)
{
	TCHAR *start = (TCHAR *)string;
	
	while (*string++)                       /* find end of string */
		;
	/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;
	
	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;
	
	return NULL;
}

static TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}

BOOL BackupFile(LPCTSTR lpszFile)
{
	ASSERT(lpszFile);
	
	// if file doesn't exist, nothing to do
	if (_taccess(lpszFile, 00) == -1)
		return TRUE;
	
	// file exists, so make a backup copy
	
	CString strFileRoot = lpszFile;
	
	CString strPath = _T("");
	CString strExt = _T("");
	BOOL bSuccess = FALSE;
	int i = 0 ; for ( i = 1; i <= 9999; i++)
	{
		strExt.Format(_T(".%04d"), i);
		strPath = strFileRoot;
		strPath += strExt;
		if (_taccess(strPath, 00) == -1)
		{
			// file doesn't exist, so we can use it
			bSuccess = CopyFile(lpszFile, strPath, TRUE);
			break;
		}
	}
	
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
// CXFileTransferDlg dialog

CXFileTransferDlg::CXFileTransferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXFileTransferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXFileTransferDlg)
	m_SenderID = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXFileTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXFileTransferDlg)
	DDX_Control(pDX, IDC_TRANSFERSTATIC, m_TransferStatic);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_Progress);
	DDX_Text(pDX, IDC_SENDERIDEDIT, m_SenderID);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXFileTransferDlg, CDialog)
	//{{AFX_MSG_MAP(CXFileTransferDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_SENDERIDEDIT, OnChangeSenderidedit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXFileTransferDlg message handlers

BOOL CXFileTransferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	if(m_FTPTransfer.Initialze() == FALSE)
	{
		AfxMessageBox("서버 주소를 읽을 수 없습니다.");
		return FALSE;
	}

	TCHAR		registrybuffer[_REGSTRING_MAXLENGTH];
	memset(registrybuffer, 0, _REGSTRING_MAXLENGTH);
	if(_XRegReadString(ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_LASTUSERNAME, "", registrybuffer, _REGSTRING_MAXLENGTH, TRUE))
		m_SenderID = registrybuffer;		
	else
		m_SenderID = _T("UNKNOWNUSER");
	
	UpdateData( FALSE );

	m_Progress.SetRange(0, 100);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXFileTransferDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXFileTransferDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CXFileTransferDlg::CompressLogFile( void )
{
	TCHAR		registrybuffer[_REGSTRING_MAXLENGTH];
	memset(registrybuffer, 0, _REGSTRING_MAXLENGTH);
	if(!_XRegReadString(ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_INSTALLEDPATH, "", registrybuffer, _REGSTRING_MAXLENGTH, TRUE))		
	{
		GetCurrentDirectory( _REGSTRING_MAXLENGTH, registrybuffer );
	}

	_chdir( registrybuffer );

	BackupFile( "Log.xl" );

	HZIP hZip = NULL;
	TCHAR logfilename[4][32] = { _T("ND_CRASHLOG.HTM"),_T("ND_DUMP.BIN"),_T("EOD.Log"),_T("XLauncher.log") };
	int nFiles = 0;
	
	int i = 0 ; for ( i = 0; i < 4; i++)
	{
		if (!hZip)
		{
			hZip = CreateZip((LPVOID)(LPCTSTR)_T("Log.xl"), 0, ZIP_FILENAME);
			
			if (!hZip)
			{
				AfxMessageBox("failed to create log.xl");
				return FALSE;
			}
		}

		ZRESULT zr = ZipAdd(hZip, logfilename[i], (LPVOID)(LPCTSTR)logfilename[i], 0, ZIP_FILENAME);
		if (zr == ZR_OK)
		{
			nFiles++;
		}
		else
		{
			CString message;
			message.Format(_T("ERROR - failed to add '%s' to zip\n"), logfilename[i]);
		}
	}

	if (hZip) CloseZip(hZip);
	return TRUE;
}

void CXFileTransferDlg::OnOK() 
{
	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));
	
	TCHAR *pszFilePart = GetFilePart(szModuleName);
	pszFilePart[0] = NULL;

	_chdir( szModuleName );

	m_Progress.ShowWindow(TRUE);
	m_TransferStatic.ShowWindow(TRUE);
	m_TransferStatic.SetWindowText("Compressing...");
	m_TransferStatic.RedrawWindow();
	CompressLogFile();
	m_Progress.SetPos(30);

	// TODO: Add extra validation here
	m_Progress.ShowWindow(TRUE);
	m_TransferStatic.ShowWindow(TRUE);
	m_TransferStatic.SetWindowText("파일을 전송중입니다.");

	if(m_FTPTransfer.ConnetServer() == FALSE)
	{
		AfxMessageBox("인터넷 연결에 실패했습니다.");
		goto exit;
	}
	m_Progress.SetPos(50);
	
	m_FTPTransfer.SetServerPath( m_SenderID );
	m_Progress.SetPos(55);

	// Look for File Path
	TCHAR		registrybuffer[_REGSTRING_MAXLENGTH];
	memset(registrybuffer, 0, _REGSTRING_MAXLENGTH);
	if(_XRegReadString(ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_INSTALLEDPATH, "", registrybuffer, _REGSTRING_MAXLENGTH, TRUE))
	{
		m_FTPTransfer.SetInputSpec(registrybuffer);
	}
	else
	{
		AfxMessageBox("파일 경로를 찾지 못했습니다.");
		goto exit;
	}

	m_Progress.SetPos(60);

	// Send EOD.log
	m_FTPTransfer.ConcatInputSpec("\\Log.xl");
	m_FTPTransfer.SetOutputSpec(_T("./Log.xl"));
	m_FTPTransfer.SendFile();
	m_Progress.SetPos(100);

	m_TransferStatic.SetWindowText("파일 전송이 완료 되었습니다.");
	goto exit;
	
exit :
	CDialog::OnOK();
}

void CXFileTransferDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}



void CXFileTransferDlg::OnChangeSenderidedit() 
{
	UpdateData();
}
