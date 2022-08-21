#pragma once


// GameOverDialog dialog

class GameOverDialog : public CDialog
{
	DECLARE_DYNAMIC(GameOverDialog)

public:
	GameOverDialog(CString* str, CWnd* pParent = nullptr);   // standard constructor
	virtual ~GameOverDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_text;
};
