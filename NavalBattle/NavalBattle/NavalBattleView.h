
// NavalBattleView.h : interface of the CNavalBattleView class
//

#pragma once

#include <winsock2.h>

//constexpr UINT IDC_CONNECT{ 1500 };
//constexpr UINT IDC_DISCONNECT{ 1503 };
//constexpr UINT IDC_PORT_EDIT{ 1501 };
//constexpr UINT IDC_IP_EDIT{ 1502 };


class CNavalBattleView : public CView
{
protected: // create from serialization only
	CNavalBattleView() noexcept;
	DECLARE_DYNCREATE(CNavalBattleView)

	// Attributes
public:


	// Operations
public:
	CNavalBattleDoc* GetDocument() const;
	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	
	// Implementation
public:
	void OpenDialog(CNavalBattleDoc* PDoc, int winner);
	virtual ~CNavalBattleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
public:
	//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
public:
	CButton m_BnConnect;
	CButton m_BnDisconnect;
	CEdit m_port;
	CEdit m_ipaddr;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // debug version in NavalBattleView.cpp
inline CNavalBattleDoc* CNavalBattleView::GetDocument() const
{
	return reinterpret_cast<CNavalBattleDoc*>(m_pDocument);
}
#endif

