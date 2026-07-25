// noticedialog.cpp : implementation file
//

#include "stdafx.h"
#include "xlauncher.h"
#include "noticedialog.h"

#include "BitmapDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoticeDialog dialog


CNoticeDialog::CNoticeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNoticeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoticeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_csIPAddress = _T("about:blank");
}


void CNoticeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoticeDialog)
	DDX_Control(pDX, IDC_CLOSEBUTTON, m_CloseButton);
	DDX_Control(pDX, IDC_WEBBOARDBROWSER, m_WebBoardBrowser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNoticeDialog, CDialog)
	//{{AFX_MSG_MAP(CNoticeDialog)
	ON_BN_CLICKED(IDC_CLOSEBUTTON, OnClosebutton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoticeDialog message handlers

BOOL CNoticeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->MoveWindow( 0, 0, 520, 430 );
	this->CenterWindow();

	m_CloseButton.SetBitmaps(IDB_NOTICECLOSEBUTTONBITMAP, RGB(255, 0, 0), IDB_NOTICECLOSEBUTTONBITMAP, RGB(255, 0, 0));
	m_CloseButton.DrawBorder(FALSE);
	m_CloseButton.DrawTransparent();
	m_CloseButton.SetFlat(TRUE);
	m_CloseButton.MoveWindow(499, 18, 14, 14);

	m_WebBoardBrowser.MoveWindow(0, 50, 520, 380);
	m_WebBoardBrowser.Navigate( m_csIPAddress );

	SetTitle( _XWEBBROWSER_TITLE_NOTICE );
				
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNoticeDialog::SetTitle( _XWEBBROWSER_TITLE titletype )
{
	if( titletype == _XWEBBROWSER_TITLE_NOTICE )
	{
		CDC* pDC = GetDC();
		if( pDC )
		{
			m_BorderImage.Create( pDC, IDB_NOTICETITLEBITMAP );
			ReleaseDC( pDC );
		}
	}
	else if( titletype == _XWEBBROWSER_TITLE_UPDATE )
	{
		CDC* pDC = GetDC();
		if( pDC )
		{
			m_BorderImage.Create( pDC, IDB_UPDATETITLEBITMAP );
			ReleaseDC( pDC );
		}
	}
}


void CNoticeDialog::ShowNoticePage( LPTSTR IPAddress, _XWEBBROWSER_TITLE titletype )
{
	m_csIPAddress = IPAddress;
	m_WebBoardBrowser.Navigate( m_csIPAddress );
	SetTitle( titletype );
	ShowWindow( SW_SHOW );
	RedrawWindow();
}

void CNoticeDialog::OnClosebutton() 
{
	// TODO: Add your control notification handler code here
	ShowWindow( SW_HIDE );
}


BOOL CNoticeDialog::OnEraseBkgnd(CDC* pDC) 
{
	/*
	CBrush pBrush;
	RECT rc;
	GetClientRect(&rc);
	pBrush.CreateSolidBrush(RGB(0,0,0));	
	pDC->FillRect(&rc,&pBrush);
	*/

	m_BorderImage.PasteToDC( pDC, 0, 0 );	
	return TRUE;//CDialog::OnEraseBkgnd(pDC);
}

void CNoticeDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
	PostMessage (WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
}
