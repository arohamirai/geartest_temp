// geartest_tempDlg.h : header file
//

#pragma once

#include "afxwin.h"

#include "Listener.h"
// Cgeartest_tempDlg dialog
class Cgeartest_tempDlg : public CDialog
{
// Construction
public:
	Cgeartest_tempDlg(CWnd* pParent = NULL);	// standard constructor
	~Cgeartest_tempDlg();

// Dialog Data
	enum { IDD = IDD_geartest_temp_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()


private:
	DShowLib::Grabber		m_cGrabber; // The instance of the Grabber class.

    DShowLib::FrameHandlerSink::tFHSPtr           m_pSink;
	CListener m_cListener;		// Image processing will be done by this object.

public:
	afx_msg void OnBnClickedButtondevice();
	afx_msg void OnBnClickedButtonimagesettings();
	void SetButtonStates(void);
private:
	CButton m_cButtonSettings;
	CButton m_cButtonLive;
public:
	afx_msg void OnBnClickedButtonlivevideo();
private:
	CStatic m_cStaticVideoWindow;

/////////////////////////////////////////

};
