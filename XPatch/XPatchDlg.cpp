// XPatchDlg.cpp : implementation file
//

#include "stdafx.h"

#include <direct.h>

#include "XPatch.h"
#include "XPatchDlg.h"

#include "RegStringDefine.h"
#include "XRegistry.h"

#include "Package.h"

#include "Zip\\ZipArchive.h"

#include "SeverMessageDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPatchDlg dialog

CXPatchDlg::CXPatchDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CXPatchDlg::IDD, pParent, "IDR_BORDERRGN")
{
	//{{AFX_DATA_INIT(CXPatchDlg)
	m_PatchPackageFilename = _T("");
	m_ProgressString = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXPatchDlg)	
	DDX_Text(pDX, IDC_SELECTEDFILENAMESTATIC, m_PatchPackageFilename);
	DDX_Text(pDX, IDC_PROGRESSSTATIC, m_ProgressString);
	DDX_Control(pDX, IDC_MINIMIZEBUTTON, m_MinimizeButton);
	DDX_Control(pDX, IDC_CLOSEBUTTON, m_CloseButton);
	DDX_Control(pDX, IDC_SELECTPACKAGEBUTTON, m_PackageOpenButton );
	DDX_Control(pDX, IDC_UPDATEBUTTON, m_PatchButton );
	DDX_Control(pDX, IDC_SELECTEDFILENAMESTATIC, m_SelectedPackageStatic );
	DDX_Control(pDX, IDC_PROGRESSSTATIC, m_ProgressStatic );	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXPatchDlg, CBitmapDialog)
	//{{AFX_MSG_MAP(CXPatchDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECTPACKAGEBUTTON, OnSelectpackagebutton)
	ON_BN_CLICKED(IDC_UPDATEBUTTON, OnUpdatebutton)	
	ON_BN_CLICKED(IDC_MINIMIZEBUTTON, OnMinimizebutton)
	ON_BN_CLICKED(IDC_CLOSEBUTTON, OnClosebutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPatchDlg message handlers

BOOL CXPatchDlg::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	CenterWindow();

	RECT rect;
	GetClientRect(&rect);
	ModifyStyle(WS_CAPTION, NULL);
	MoveWindow(&rect);
	
	MoveWindow( 0, 0, 578, 123 );
		
	LoadBitmap (IDB_BACKGROUNDBITMAP);
	SetTransparent (TRUE);
	SetTransColor (RGB(255,0,0));
	SetStaticTransparent (TRUE);
	SetClickAnywhereMove (TRUE);

	GetWindowRect( &rect );	

	m_MinimizeButton.SetIcon( IDI_MINIMIZEICON );
	m_MinimizeButton.MoveWindow( rect.right - 48, 4, 16, 16 );

	m_CloseButton.SetIcon( IDI_CLOSEICON );
	m_CloseButton.MoveWindow( rect.right - 30, 4, 16, 16 );

	m_PatchButton.SetBitmaps( IDB_UPGRADEBITMAP, RGB(255,0,0) );
	m_PatchButton.MoveWindow( 464, 92, 68, 26 );

	m_PackageOpenButton.SetBitmaps( IDB_OPENBITMAP, RGB(255,0,0) );
	m_PackageOpenButton.MoveWindow( 317, 92, 138, 26 );

	m_SelectedPackageStatic.MoveWindow( 54, 44, 472, 15 );
	m_SelectedPackageStatic.SetTextColor(RGB(255,255,255))
						   .SetBkColor( RGB(0,0,0) );
						   //.SetTransparent(TRUE);	
	m_ProgressStatic.MoveWindow( 54, 69, 472, 15 );
	m_ProgressStatic.SetTextColor(RGB(255,255,255))
				    .SetBkColor( RGB(0,0,0) );
	
	m_Message = _T("");

	m_PatchNumber = 0;
	m_ServerPatchNumber = 0;
	
	m_Version = 0;
	m_ServerVersion = 0;
	
	CString intializemessage;
	m_PatchPackageFilename = _T("[ 구룡쟁패를 업그레이드 합니다. ]");

	intializemessage.Format( "현재 설치된 구룡쟁패 패치버전은 %d입니다. ", _XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_PATCHNUMBER, 0, FALSE ) );
	SetProgressString( intializemessage );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CBitmapDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXPatchDlg::OnPaint() 
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
		CBitmapDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXPatchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CXPatchDlg::OnSelectpackagebutton() 
{
	// TODO: Add your control notification handler code here
	CFileDialog	FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL );

	FileDlg.m_ofn.hwndOwner		= NULL;
	FileDlg.m_ofn.lStructSize		= sizeof(OPENFILENAME);
	FileDlg.m_ofn.lpstrFilter		= "XPZ File(*.XPZ)\0*.XPZ\0";
	FileDlg.m_ofn.lpstrInitialDir	= ".\\";
	FileDlg.m_ofn.lpstrTitle		= "Load NineDragons Upgrade package file";
	FileDlg.m_ofn.lpstrDefExt		= "*.XPZ";
	
	if( FileDlg.DoModal() == IDOK )
	{			
		this->m_PatchPackageFilename = FileDlg.GetPathName();
		m_SelectedPackageStatic.SetTextColor(RGB(255,255,255))
							   .SetBkColor( RGB(0,0,0) )			   
						       .SetTransparent(TRUE);
		
		UpdateData( FALSE );
		RedrawWindow();
	}
	
}

void CXPatchDlg::OnMinimizebutton() 
{
	// TODO: Add your control notification handler code here
	ShowWindow( SW_MINIMIZE );
}

void CXPatchDlg::OnClosebutton() 
{
	// TODO: Add your control notification handler code here
	PostQuitMessage( IDOK );
}


void CXPatchDlg::OnUpdatebutton() 
{
	// TODO: Add your control notification handler code here
	Patch();

}

void CXPatchDlg::Patch()
{
	 // Check registry
	 char registrybuffer[_REGSTRING_MAXLENGTH];    	
	 memset( registrybuffer, 0, _REGSTRING_MAXLENGTH );	 
	 _XRegReadString( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_INSTALLEDPATH, "", registrybuffer, _REGSTRING_MAXLENGTH, FALSE);
	 
	 BOOL showedservermessage = TRUE;

	 if( strlen( registrybuffer ) <= 0 )
	 {
		 CString tMessageString;
		 tMessageString.LoadString( IDS_NOTINSTALLEDSTRING );

		 AfxMessageBox( tMessageString );
		 return;

	 }

	 /*if( this->m_PatchPackageFilename.GetLength() == 0 ) 
	 {
		 OnSelectpackagebutton();

		 if( this->m_PatchPackageFilename.GetLength() == 0 ) return;
	 }*/

	 char cwdpath[ _MAX_PATH ];
	 GetCurrentDirectory( _MAX_PATH, cwdpath );
	 
	 // Decompress patch package...	 

	 CZipArchive zip;

	 m_PatchNumber = _XRegReadInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_PATCHNUMBER, 0, FALSE );

	 int patchprocessindex = m_PatchNumber + 1;
	 
	 while(1)
	 {		
		 this->m_PatchPackageFilename.Format( "PATCH%d.XPZ", patchprocessindex );
	
		 _chdir( cwdpath );
		 
		 if( !CheckExistFile( this->m_PatchPackageFilename, TRUE ) )
		 {
			 m_PatchPackageFilename = _T("[업그레이드 작업 완료]");
			 CString messagestring;
			 messagestring.Format( "[%d]번째 패치작업이 끝났습니다. 구룡쟁패를 이용하여 주셔서 감사합니다.", patchprocessindex );
			 SetProgressString( messagestring );
			 break;
		 }

		 zip.Open( this->m_PatchPackageFilename, CZipArchive::openReadOnly );

		 _chdir( registrybuffer );

		 for ( int i = 0; i < zip.GetNoEntries(); i++)
		 {
			  zip.ExtractFile(i, registrybuffer);
		 }

		 zip.Close();

		 if( ParsingPatchScript() == -1 )
		 {			 
			 _chdir( cwdpath );	 
			 return;
		 }

		 
		 // Package 안의 파일 이름 설정 
		 CPackage package;
		 CString patchfilename;
		 patchfilename.Format( "PATCH%d.XP", m_ServerPatchNumber );

		 CString patchinfofilename;
		 patchinfofilename.Format( "PATCH%d.XPI", m_ServerPatchNumber );

		 
		 if( m_PatchNumber >= m_ServerPatchNumber )
		 {
			CString tmessagestring;
			tmessagestring.LoadString( IDS_VERSIONCHECKSTRING );
			if( AfxMessageBox( tmessagestring, MB_YESNO ) != IDYES )
			{			
				if( CheckExistFile( patchfilename, TRUE ) ) CFile::Remove( patchfilename );
				if( CheckExistFile( patchinfofilename, TRUE ) ) CFile::Remove( patchinfofilename );			
				_chdir( cwdpath );
				return;
			}
		 } 


		 // 메세지 출력
		 int result = -1;

		 result = IDYES;

		 if( showedservermessage )
		 {
			 showedservermessage = FALSE;
		
			 if( m_Message.GetLength() > 0 )
			 {
				 CSeverMessageDialog messagedialog;	
				 messagedialog.m_MessageString = this->m_Message;
				 result = messagedialog.DoModal();
				 
				 if( result == IDOK ) result = IDYES;
				 else result = -1;
				
				 this->m_Message = _T("");
			 }
			 else
			 {		 
				 CString versionstring;
				 versionstring.Format( "%d", this->m_ServerPatchNumber );
				 AfxFormatString1(this->m_Message, IDS_UPDATENOW, versionstring );		 
				 result = AfxMessageBox( this->m_Message );
			 }
		 }

		 if( result == IDYES ) 
		 {
			 if( package.Patch( patchfilename, patchinfofilename ) )
			 {	
				 CString messagestring;
				 messagestring.Format( "[%d]번 업그레이드가 끝났습니다.", patchprocessindex );
				 SetProgressString( messagestring );
				 // 패치된 버전 번호 및 패치 넘버를 레지스트리에 등록
				 _XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_PATCHVERSION, this->m_ServerVersion );
				 _XRegWriteInt( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_PATCHNUMBER, m_ServerPatchNumber );
				 _XRegWriteString( ID_REGSTR_MASTERKEY, ID_REGSTR_INDY21KEY, ID_REGSTR_PACKAGEVERSION, m_ServerPackageVersionName );
				 
			 }
			 else
			 {
				 CString messagestring;
				 messagestring.Format( "[%d]번 업그레이드가 끝났습니다.", patchprocessindex );
				 SetProgressString( messagestring );
			 }
		 }
		 
		 if( CheckExistFile( patchfilename, TRUE ) ) CFile::Remove( patchfilename );
		 if( CheckExistFile( patchinfofilename, TRUE ) ) CFile::Remove( patchinfofilename );	 

		 patchprocessindex++;
	 }

	 _chdir( cwdpath );
}


int CXPatchDlg::ParsingPatchScript( void )
{
	FILE* PatchScriptFile = NULL;

	PatchScriptFile = fopen( "Patch.PSC", "rt" );

	if( !PatchScriptFile )
	{
		CString tMessageString;
		tMessageString.LoadString( IDS_PATCHINFORMATIONFILENOTEXIST );
		AfxMessageBox( tMessageString );
		// File openning error...
		return -1;
	}

	char argIDString[256];
	char argValueString[256];
	char parsingString[256];
	BOOL messagestart = FALSE;

	m_Message = _T("");
	//int  argValue;

	while( !feof( PatchScriptFile ) )
	{	
		if( feof( PatchScriptFile ) ) break;
				
		memset( parsingString, 0, 256 );
		fgets( parsingString, 255, PatchScriptFile );

		if( parsingString[0] != ';' ) 
		{
			memset( argIDString, 0, 256 );
			memset( argValueString, 0, 256 );
			sscanf( parsingString, "%s %s", argIDString, argValueString );
			
			if( messagestart )
			{
				if( strcmp( argIDString, "ID_ENDMESSAGE" ) == 0 )
				{
					messagestart = FALSE;
				}
				else
				{
					CString tstr = parsingString;
					tstr.Replace( "\n", "\r\n" );
					m_Message += tstr;					
				}
			}
			else
			{
				if( strcmp( argIDString, "ID_PATCHINDEX" ) == 0 )
				{
					m_ServerPatchNumber = atoi( argValueString );
				}
				if( strcmp( argIDString, "ID_PACKAGEVERSION" ) == 0 )
				{
					m_ServerPackageVersionName = argValueString;
				}
				else if( strcmp( argIDString, "ID_PATCHVERSION" ) == 0 )
				{
					m_ServerVersion = atoi( argValueString );
				}			
				else if( strcmp( argIDString, "ID_MESSAGE" ) == 0 )
				{
					messagestart = TRUE;			
				}
			}
		}		
	}

	fclose( PatchScriptFile );
	
	if( CheckExistFile( "Patch.PSC", TRUE ) ) CFile::Remove( "Patch.PSC" );

	return 1;
}

void CXPatchDlg::SplitMessage( CString& sourceString, CString& destString )
{
	char tbuffer[512];

	memset( tbuffer, 0, 512 );

	strncpy( tbuffer, sourceString, 511 );

	int i = 0 ; for(  i = 0; i < 511; i++ )
	{
		if( tbuffer[i] != ' ' ) break;
	}

	destString = tbuffer + i;
	
}


void CXPatchDlg::SetProgressString( CString progressmessage )
{	
	m_SelectedPackageStatic.SetTextColor(RGB(255,255,255))
						   .SetBkColor( RGB(0,0,0) )
						   .SetTransparent(FALSE);

	this->m_ProgressString = progressmessage;
	m_ProgressStatic.SetTextColor(RGB(255,255,255))
					.SetBkColor( RGB(0,0,0) )
					.SetTransparent(FALSE);
	
	RECT windowrect;
	this->GetWindowRect( &windowrect );

	windowrect.right	 = windowrect.left + 527;
	windowrect.bottom	 = windowrect.top + 82;
	windowrect.left		 += 55;
	windowrect.top		 += 41;							
		
	UpdateData( FALSE );
	RedrawWindow( &windowrect );
	m_SelectedPackageStatic.RedrawWindow();
	m_ProgressStatic.RedrawWindow();
}
