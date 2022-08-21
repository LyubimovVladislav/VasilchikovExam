// GameOverDialog.cpp : implementation file
//

#include "pch.h"
#include "NavalBattle.h"
#include "GameOverDialog.h"
#include "afxdialogex.h"


// GameOverDialog dialog

IMPLEMENT_DYNAMIC(GameOverDialog, CDialog)

GameOverDialog::GameOverDialog(CString* str, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG1, pParent)
	, m_text(*str)
{

}

GameOverDialog::~GameOverDialog()
{
}

void GameOverDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_text);
}


BEGIN_MESSAGE_MAP(GameOverDialog, CDialog)
	ON_BN_CLICKED(IDOK, &GameOverDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &GameOverDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// GameOverDialog message handlers


void GameOverDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	CDialog::OnOK();
}


void GameOverDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


