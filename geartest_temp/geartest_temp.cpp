// geartest_temp.cpp : Implements the classes for the application.
//

#include "stdafx.h"
#include "geartest_temp.h"
#include "geartest_tempDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cgeartest_tempApp

BEGIN_MESSAGE_MAP(Cgeartest_tempApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// Cgeartest_tempApp construction

Cgeartest_tempApp::Cgeartest_tempApp()
{
}


// The one and only Cgeartest_tempApp object

Cgeartest_tempApp theApp;


// Cgeartest_tempApp initialization

BOOL Cgeartest_tempApp::InitInstance()
{

	// Init IC Imaging Control 

	if( !DShowLib::InitLibrary() )
	{
		AfxMessageBox( TEXT("The IC Imaging Control Class Library could not be initialized.\n(invalid license key?)") );
		exit( 1 );
	}
	// At the end of the program, the IC Imaging Control Class Library must be cleaned up
	// by a call to ExitLibrary().
	atexit(DShowLib::ExitLibrary );

	// InitCommonControls() is required on Windows XP, if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("geartest_temp"));

	Cgeartest_tempDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();


	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}
