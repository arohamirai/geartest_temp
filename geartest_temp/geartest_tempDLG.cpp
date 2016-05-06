// geartest_tempDlg.cpp : implementation file
//

#include "stdafx.h"
#include "geartest_temp.h"
#include "geartest_tempDlg.h"
#include ".\geartest_tempdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Save some typing...
using namespace DShowLib;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cgeartest_tempDlg dialog



Cgeartest_tempDlg::Cgeartest_tempDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cgeartest_tempDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


Cgeartest_tempDlg::~Cgeartest_tempDlg()
{
}

void Cgeartest_tempDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTONIMAGESETTINGS, m_cButtonSettings);
	DDX_Control(pDX, IDC_BUTTONLIVEVIDEO, m_cButtonLive);
	DDX_Control(pDX, IDC_STATICVIDEO, m_cStaticVideoWindow);
}

// Cgeartest_tempDlg message handlers

BEGIN_MESSAGE_MAP(Cgeartest_tempDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTONDEVICE, OnBnClickedButtondevice)
	ON_BN_CLICKED(IDC_BUTTONIMAGESETTINGS, OnBnClickedButtonimagesettings)
	ON_BN_CLICKED(IDC_BUTTONLIVEVIDEO, OnBnClickedButtonlivevideo)
	ON_WM_CLOSE()
END_MESSAGE_MAP()



BOOL Cgeartest_tempDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// Resize the video window to 640*480 pixels.
	m_cStaticVideoWindow.SetWindowPos(NULL,0,0,640,480,SWP_NOMOVE|SWP_NOZORDER);


	// Add the CListener object to the the Grabber object.
	m_cGrabber.addListener(&m_cListener);

	m_cListener.SetParent(this);

	// Pass the video window to the listener, so it can draw in it.
	m_cListener.SetViewCWnd(&m_cStaticVideoWindow);
	// Set the sink
	m_pSink = FrameHandlerSink::create( DShowLib::eRGB8, 3 );
	m_pSink->setSnapMode( false ); // Automatically copy every frame to the sink and call CListener::frameReady().
	m_cGrabber.setSinkType( m_pSink );

	// Try to load the previously used video capture device.
	if( m_cGrabber.loadDeviceStateFromFile( "device.xml"))
	{
		// Display the device's name in the caption bar of the application.
		SetWindowText( TEXT("geartest_temp ") + CString(m_cGrabber.getDev().c_str()));
		m_cGrabber.startLive(false); // The live video will be displayed by the CListener object.
	}

	SetButtonStates();

	return TRUE;  // return TRUE unless you set the focus to a control.
}

void Cgeartest_tempDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
//////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon. For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//////////////////////////////////////////////////////////////////////////

void Cgeartest_tempDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle.
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
//////////////////////////////////////////////////////////////////////////
// The system calls this function to obtain the cursor to display while
// the user dragsthe minimized window.
//////////////////////////////////////////////////////////////////////////

HCURSOR Cgeartest_tempDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////
// Show the device select dialog, if the "Device" button has been clicked.
// If live video is displayed, it has to be stopped.
//////////////////////////////////////////////////////////////////////////

void Cgeartest_tempDlg::OnBnClickedButtondevice()
{
	// If live video is running, stop it.
	if(m_cGrabber.isDevValid() && m_cGrabber.isLive())
	{
		m_cGrabber.stopLive();
	}

	m_cGrabber.showDevicePage(this->m_hWnd);

	// If we have selected a valid device, save it to the file "device.xml", so
	// the application can load it automatically when it is started the next time.
	if( m_cGrabber.isDevValid())
	{
		m_cGrabber.saveDeviceStateToFile("device.xml");
	}

	// Now display the device's name in the caption bar of the application.
	SetWindowText( TEXT("geartest_temp ") + CString(m_cGrabber.getDev().c_str()));

	SetButtonStates();
}

//////////////////////////////////////////////////////////////////////////
// Show the image settings dialog of IC Imaging Control. 
//////////////////////////////////////////////////////////////////////////

void Cgeartest_tempDlg::OnBnClickedButtonimagesettings()
{
	if( m_cGrabber.isDevValid())
	{
		m_cGrabber.showVCDPropertyPage(this->m_hWnd);
		m_cGrabber.saveDeviceStateToFile("device.xml");
	}
	
}

//////////////////////////////////////////////////////////////////////////
// This method sets the states of the dialog's buttons.
//////////////////////////////////////////////////////////////////////////

void Cgeartest_tempDlg::SetButtonStates(void)
{
	bool bDevValid =  m_cGrabber.isDevValid();
	bool bIsLive   = m_cGrabber.isLive();

	m_cButtonSettings.EnableWindow(bDevValid);
	m_cButtonLive.EnableWindow(bDevValid);
	
	if( !bDevValid )
	{
		m_cButtonLive.SetWindowText(TEXT("Live Start"));
	}


	if( bIsLive )
	{
		m_cButtonLive.SetWindowText(TEXT("Live Stop"));
	}
	else
	{
		m_cButtonLive.SetWindowText(TEXT("Live Start"));
	}


}

//////////////////////////////////////////////////////////////////////////
// This method starts and stops the live video.
//////////////////////////////////////////////////////////////////////////

void Cgeartest_tempDlg::OnBnClickedButtonlivevideo()
{
	if( m_cGrabber.isDevValid())
	{
		if (m_cGrabber.isLive())
		{
			m_cGrabber.stopLive();
		}
		else
		{
			// Call startLive with "false", because the live display is done
			// by the CListener object.
			m_cGrabber.startLive(false);
		}
		SetButtonStates();
	}
}

//////////////////////////////////////////////////////////////////////////

void Cgeartest_tempDlg::OnClose()
{
	m_cGrabber.stopLive();

	CDialog::OnClose();
}


