// XPatchDlg.h : header file
//

#if !defined(AFX_XPATCHDLG_H__149E8C4B_13EC_469B_8FD8_0A613614F0AC__INCLUDED_)
#define AFX_XPATCHDLG_H__149E8C4B_13EC_469B_8FD8_0A613614F0AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BitmapDialog.h"
#include "Label.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CXPatchDlg dialog

class CXPatchDlg : public CBitmapDialog
{
// Construction
public:
	void Patch( void );
	CXPatchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXPatchDlg)
	enum { IDD = IDD_XPATCH_DIALOG };
	CString	m_PatchPackageFilename;
	CString	m_ProgressString;
	//}}AFX_DATA

	CButtonST m_CloseButton;
	CButtonST m_MinimizeButton;

	CButtonST m_PackageOpenButton;
	CButtonST m_PatchButton;

	CLabel    m_SelectedPackageStatic;
	CLabel    m_ProgressStatic;
	
	CString   m_Message;

	DWORD     m_PatchNumber;
	DWORD     m_ServerPatchNumber;
	
	DWORD	  m_Version;
	DWORD	  m_ServerVersion;

	CString   m_ServerPackageVersionName;

	int       ParsingPatchScript( void );
	void      SplitMessage( CString& sourceString, CString& destString );
	void      SetProgressString( CString progressmessage );
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXPatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelectpackagebutton();
	afx_msg void OnUpdatebutton();	
	afx_msg void OnMinimizebutton();
	afx_msg void OnClosebutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPATCHDLG_H__149E8C4B_13EC_469B_8FD8_0A613614F0AC__INCLUDED_)
