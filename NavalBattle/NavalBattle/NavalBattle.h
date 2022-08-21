
// NavalBattle.h : main header file for the NavalBattle application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CNavalBattleApp:
// See NavalBattle.cpp for the implementation of this class
//

class CNavalBattleApp : public CWinApp
{
public:
	CNavalBattleApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CNavalBattleApp theApp;
