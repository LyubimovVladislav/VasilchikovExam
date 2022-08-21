
// NavalBattleView.cpp : implementation of the CNavalBattleView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "NavalBattle.h"
#endif

#include "NavalBattleDoc.h"
#include "NavalBattleView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "GameOverDialog.h"


// CNavalBattleView

#define DEFAULT_PORT	5150
#define DEFAULT_BUFFER	2048

bool m_IsConnected;
bool m_isStarted;
SOCKET	m_sClient;
UINT ListenThread(PVOID lpParam);
void TcpSendMessage(char* szMessage);
void TcpSendMessage2(int** szMessage);
CNavalBattleView* PView;
CNavalBattleDoc* pNBDoc;
int m_currentPlayer;




IMPLEMENT_DYNCREATE(CNavalBattleView, CView)

BEGIN_MESSAGE_MAP(CNavalBattleView, CView)
	//	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CONNECT, &CNavalBattleView::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CNavalBattleView::OnBnClickedDisconnect)
END_MESSAGE_MAP()

// CNavalBattleView construction/destruction

CNavalBattleView::CNavalBattleView() noexcept
{
	// TODO: add construction code here

}

CNavalBattleView::~CNavalBattleView()
{
}

BOOL CNavalBattleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CNavalBattleView drawing

void CNavalBattleView::OnDraw(CDC* pDC)
{
	CNavalBattleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	pDC->Rectangle(100, 100, 600, 600);
	pDC->Rectangle(700, 100, 1200, 600);


	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			pDC->Rectangle(100 + (50 * i), 100 + (50 * j), 100 + (50 * (i + 1)), 100 + (50 * (j + 1)));
			pDC->Rectangle(700 + (50 * i), 100 + (50 * j), 700 + (50 * (i + 1)), 100 + (50 * (j + 1)));
		}
	}



	pDC->DrawText(_T("Ваша доска"), new CRect(100, 50, 600, 100), 1);
	pDC->DrawText(_T("Доска противника"), new CRect(700, 50, 1200, 100), 1);

	if (m_currentPlayer == 0)
	{
		pDC->SetTextColor(RGB(0, 255, 0));
		pDC->DrawText(_T("Ваш ход"), new CRect(550, 20, 750, 50), 1);
		pDC->SetTextColor(RGB(255, 255, 255));
	}
	if (m_currentPlayer == 1)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->DrawText(_T("Ход противника"), new CRect(550, 20, 750, 50), 1);
		pDC->SetTextColor(RGB(255, 255, 255));
	}

	if (m_IsConnected && m_isStarted)
	{
		int Index = pDoc->GetSquaresAmount();
		while (Index--)
			pDoc->GetSquare(Index)->Draw(pDC);
	}

}


// CNavalBattleView diagnostics

#ifdef _DEBUG
void CNavalBattleView::AssertValid() const
{
	CView::AssertValid();
}

void CNavalBattleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNavalBattleDoc* CNavalBattleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNavalBattleDoc)));
	return (CNavalBattleDoc*)m_pDocument;
}
#endif //_DEBUG


// CNavalBattleView message handlers





void CNavalBattleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default


	CClientDC ClientDC(this);
	if (point.x > 700 && point.x < 1200 && point.y>100 && point.y < 600) {
		CNavalBattleDoc* PDoc = GetDocument();
		int tempX = (point.x - 700) / 10;
		int tempY = (point.y - 100) / 10;
		while (tempX % 10 != 0 && tempX % 10 != 5)
			tempX--;
		while (tempY % 10 != 0 && tempY % 10 != 5)
			tempY--;

		int coordX = 0;
		int coordY = 0;

		while (tempX > 0) {
			tempX -= 5;
			coordX++;
		}
		while (tempY > 0) {
			tempY -= 5;
			coordY++;
		}


		
		if (m_currentPlayer != 0)
			return;
		//Проверка, было ли уже выбрано это поле
		if (PDoc->Check(coordX, coordY))
			return;

		tempX = coordX * 5 * 10 + 700;
		tempY = coordY * 5 * 10 + 100;
		CRect* rect = new CRect(tempX + 1, tempY + 1, tempX + 50 - 1, tempY + 50 - 1);

		char Str[1024];
		sprintf_s(Str, sizeof(Str), "%d%d", coordX, coordY);
		if (m_currentPlayer == 0)
		{
			TcpSendMessage(Str);
		}

		/*CPoint point;
		if (!PDoc->Turn(coordX, coordY)) {
			int** field = PDoc->getNavalBattle()->getPlayer2Field();
			if (field[coordX][coordY] != 3)
			{
				PDoc->AddSquare(new CSquare(rect, 255, 0, 0));
				ClientDC.FillRect(new CRect(tempX + 1, tempY + 1, tempX + 50 - 1, tempY + 50 - 1), new CBrush(RGB(255, 0, 0)));
			}
			int Index;
			int** field2;
			boolean bol;
			boolean turn = true;
			while (turn) {
				if (!PDoc->enemyTurn())
					turn = false;
				field2 = PDoc->getNavalBattle()->getPlayer1Field();
				for (int i = 0; i < 10; i++)
					for (int j = 0; j < 10; j++) {
						switch (field2[i][j])
						{
						case 2:
							Index = PDoc->GetSquaresAmount();
							bol = false;
							while (Index--)
								if (PDoc->GetSquare(Index)->m_rect->TopLeft() == CPoint(100 + 50 * i + 1, 100 + 50 * j + 1))
									bol = true;
							if (bol)
								continue;
							PDoc->AddSquare(new CSquare(new CRect(100 + 50 * i + 1, 100 + 50 * j + 1, 150 + 50 * i - 1, 150 + 50 * j - 1), 255, 0, 0));
							ClientDC.FillRect(new CRect(100 + 50 * i + 1, 100 + 50 * j + 1, 150 + 50 * i - 1, 150 + 50 * j - 1), new CBrush(RGB(255, 0, 0)));
							break;
						case 3:
							Index = PDoc->GetSquaresAmount();
							bol = false;
							while (Index--)
								if (PDoc->GetSquare(Index)->m_rect->TopLeft() == CPoint(100 + 50 * i + 1, 100 + 50 * j + 1)) {
									bol = true;
									break;
								}
							if (bol && !PDoc->GetSquare(Index)->isGray())
								continue;
							PDoc->ReplaceSquare(Index, new CSquare(new CRect(100 + 50 * i + 1, 100 + 50 * j + 1, 150 + 50 * i - 1, 150 + 50 * j - 1), 0, 255, 0));
							ClientDC.FillRect(new CRect(100 + 50 * i + 1, 100 + 50 * j + 1, 150 + 50 * i - 1, 150 + 50 * j - 1), new CBrush(RGB(0, 255, 0)));
							break;
						default:
							break;
						}
					}
			}
			if (PDoc->getNavalBattle()->gameFinished()) {
				OpenDialog(PDoc, 0);
			}
		}
		else
		{
			boolean bol;
			int Index;
			int** field = PDoc->getNavalBattle()->getPlayer2Field();
			for (int i = 0; i < 10; i++)
				for (int j = 0; j < 10; j++) {
					switch (field[i][j])
					{
						break;
					case 2:
						bol = false;
						Index = PDoc->GetSquaresAmount();
						while (Index--)
							if (PDoc->GetSquare(Index)->m_rect->TopLeft() == CPoint(700 + 50 * i + 1, 100 + 50 * j + 1))
								bol = true;
						if (bol)
							continue;
						PDoc->AddSquare(new CSquare(new CRect(700 + 50 * i + 1, 100 + 50 * j + 1, 750 + 50 * i - 1, 150 + 50 * j - 1), 255, 0, 0));
						ClientDC.FillRect(new CRect(700 + 50 * i + 1, 100 + 50 * j + 1, 750 + 50 * i - 1, 150 + 50 * j - 1), new CBrush(RGB(255, 0, 0)));
						break;
					case 3:
						bol = false;
						Index = PDoc->GetSquaresAmount();
						while (Index--)
							if (PDoc->GetSquare(Index)->m_rect->TopLeft() == CPoint(700 + 50 * i + 1, 100 + 50 * j + 1))
								bol = true;
						if (bol)
							continue;
						PDoc->AddSquare(new CSquare(new CRect(700 + 50 * i + 1, 100 + 50 * j + 1, 750 + 50 * i - 1, 150 + 50 * j - 1), 0, 255, 0));
						ClientDC.FillRect(new CRect(700 + 50 * i + 1, 100 + 50 * j + 1, 750 + 50 * i - 1, 150 + 50 * j - 1), new CBrush(RGB(0, 255, 0)));
						break;
					default:
						break;
					}
				}
			if (PDoc->getNavalBattle()->gameFinished()) {
				OpenDialog(PDoc, 1);
			}
		}*/
	}


	CView::OnLButtonUp(nFlags, point);
}

void CNavalBattleView::OpenDialog(CNavalBattleDoc* PDoc, int winner) {
	/*CDialog* GameOver = new CDialog(IDD_DIALOG1, this );*/
	GameOverDialog* GameOver;
	if (!winner)
		GameOver = new GameOverDialog(new CString("You won"), this);
	else
		GameOver = new GameOverDialog(new CString("You lost"), this);
	INT_PTR nRet = -1;
	nRet = GameOver->DoModal();
	OnBnClickedDisconnect();

}


void CNavalBattleView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_currentPlayer = 2;
	CNavalBattleDoc* PDoc = GetDocument();
	PView = this;
	pNBDoc = PDoc;
	PDoc->NbInit();
	char Str[128];
	sprintf_s(Str, sizeof(Str), "%d", DEFAULT_PORT);
	m_port.SetWindowTextA(Str);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(false);
	m_ipaddr.SetWindowTextA("localhost");
	m_isStarted = false;
	m_IsConnected = false;
}


int CNavalBattleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	m_BnConnect.Create("Подключиться", BS_PUSHBUTTON, CRect(1225, 500, 1350, 540), this, IDC_CONNECT);
	m_BnConnect.ShowWindow(SW_SHOW);
	m_BnDisconnect.Create("Отключиться", BS_PUSHBUTTON, CRect(1225, 560, 1350, 600), this, IDC_DISCONNECT);
	m_BnDisconnect.ShowWindow(SW_SHOW);

	m_port.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP |
		ES_AUTOHSCROLL | WS_BORDER, CRect(1225, 450, 1350, 490), this, IDC_PORT_EDIT);
	m_port.SetFocus();

	m_ipaddr.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP |
		ES_AUTOHSCROLL | WS_BORDER, CRect(1225, 400, 1350, 440), this, IDC_IP_EDIT);
	m_ipaddr.SetFocus();

	return 0;
}


void CNavalBattleView::OnBnClickedConnect()
{

	//GetDlgItem(IDC_START)->EnableWindow(false);

	//
	char szServer[128];	// Имя или IP-адрес сервера
	int	iPort;			// Порт

	WSADATA	wsd;

	struct sockaddr_in 	server;
	struct hostent* host = NULL;

	char Str[256];

	GetDlgItem(IDC_IP_EDIT)->GetWindowText(szServer, sizeof(szServer));
	GetDlgItem(IDC_PORT_EDIT)->GetWindowText(Str, sizeof(Str));
	iPort = atoi(Str);

	if (iPort <= 0 || iPort >= 0x10000)
	{
		AfxMessageBox("Incorrect Port number");
		return;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		AfxMessageBox("Failed to load Winsock library!");
		return;
	}

	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sClient == INVALID_SOCKET)
	{
		sprintf_s(Str, sizeof(Str), "socket() failed: %d\n", WSAGetLastError());
		AfxMessageBox(Str);
		return;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szServer);

	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(szServer);
		if (host == NULL)
		{
			sprintf_s(Str, sizeof(Str), "Unable to resolve server: %s", szServer);
			AfxMessageBox(Str);
			return;
		}
		CopyMemory(&server.sin_addr, host->h_addr_list[0],
			host->h_length);
	}
	if (connect(m_sClient, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "connect() failed: %d", WSAGetLastError());
		AfxMessageBox(Str);
		return;
	}

	m_IsConnected = true;
	GetDlgItem(IDC_CONNECT)->EnableWindow(!m_IsConnected);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(m_IsConnected);
	AfxBeginThread(ListenThread, NULL);
	TcpSendMessage2(pNBDoc->getNavalBattle()->getPlayer1Field());
	//
}

void CNavalBattleView::OnBnClickedDisconnect()
{
	m_currentPlayer = 2;
	m_isStarted = false;
	m_IsConnected = false;
	closesocket(m_sClient);
	pNBDoc->OnEditClearAll();

	WSACleanup();
	GetDlgItem(IDC_CONNECT)->EnableWindow(!m_IsConnected);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(m_IsConnected);
	
	PView->Invalidate();
	
}

UINT ListenThread(PVOID lpParam)
{
	BOOL	bSendOnly = FALSE;	// Только отправка данных
	char szBuffer[DEFAULT_BUFFER];
	int	ret;

	do {

		ret = recv(m_sClient, szBuffer, DEFAULT_BUFFER, 0);

		//char temp[128];
		//sprintf_s(temp, sizeof(temp), "client: %d", ret);
		//AfxMessageBox(temp);

		//AfxMessageBox()

		if (ret > 0)
		{

			if (!m_isStarted && ret == 1)
			{
				if (szBuffer[0] == '0')
					m_currentPlayer = 0;
				else if (szBuffer[0] == '1')
					m_currentPlayer = 1;
				else
					continue;
				m_isStarted = true;
				PView->Invalidate();
			}

			if (ret > 1)
			{
				/*AfxMessageBox(szBuffer);*/
				if (szBuffer[0] == 'E')
				{
					if (szBuffer[1] == '1')
						PView->OpenDialog(pNBDoc, 1);
					else
						PView->OpenDialog(pNBDoc, 0);
					break;
				}
				if (m_currentPlayer == 0)
				{
					int i = (int)szBuffer[0] - (int)'0';
					int j = (int)szBuffer[1] - (int)'0';
					if (szBuffer[2] == '0')
						pNBDoc->AddSquare(new CSquare(new CRect(700 + 50 * i + 1, 100 + 50 * j + 1, 750 + 50 * i - 1, 150 + 50 * j - 1), 0, 255, 0));
					else
					{
						pNBDoc->AddSquare(new CSquare(new CRect(700 + 50 * i + 1, 100 + 50 * j + 1, 750 + 50 * i - 1, 150 + 50 * j - 1), 255, 0, 0));
						m_currentPlayer = 1;
					}
					PView->Invalidate();
				}
				else
				{
					int i = (int)szBuffer[0] - (int)'0';
					int j = (int)szBuffer[1] - (int)'0';
					int Index;
					bool bol = false;
					Index = pNBDoc->GetSquaresAmount();
					if (szBuffer[2] == '0')
					{
						pNBDoc->AddSquare(new CSquare(new CRect(100 + 50 * i + 1, 100 + 50 * j + 1, 150 + 50 * i - 1, 150 + 50 * j - 1), 255, 0, 0));
						m_currentPlayer = 0;
					}
					else {
						while (Index--)
							if (pNBDoc->GetSquare(Index)->m_rect->TopLeft() == CPoint(100 + 50 * i + 1, 100 + 50 * j + 1)) {
								bol = true;
								break;
							}
						if (bol && !pNBDoc->GetSquare(Index)->isGray())
							continue;
						pNBDoc->ReplaceSquare(Index, new CSquare(new CRect(100 + 50 * i + 1, 100 + 50 * j + 1, 150 + 50 * i - 1, 150 + 50 * j - 1), 0, 255, 0));
					}
					PView->Invalidate();
				}
			}
		}

		else if (ret == 0)
		{
			AfxMessageBox("Connection closed\n");
			return 1;
		}
		/*else if (m_IsConnected)
		{
			AfxMessageBox("recv failed : % d\n", WSAGetLastError());
		}*/

	} while (m_IsConnected);

	return 0;
}

void TcpSendMessage(char* message) {
	/*char szMessage[1024];	*/	// Сообщение для отправки

	//char szBuffer[DEFAULT_BUFFER];
	int	ret;
	char Str[256];

	/*CDocument::UpdateAllViews(NULL);*/

	// Отправка данных 

	ret = send(m_sClient, message, strlen(message) + 1, 0);

	if (ret == 0)
		return;
	else if (ret == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "send() failed: %d", WSAGetLastError());
		AfxMessageBox(Str);
		return;
	}
}

void TcpSendMessage2(int** message) {
	char szMessage[1024];		// Сообщение для отправки


	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
			if (message[i][j] == 0)
				szMessage[i * 10 + j] = '0';
			else
				szMessage[i * 10 + j] = '1';
	}
	int	ret;
	char Str[256];



	ret = send(m_sClient, szMessage, strlen(szMessage), 0);

	if (ret == 0)
		return;
	else if (ret == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "send() failed: %d", WSAGetLastError());
		AfxMessageBox(Str);
		return;
	}
}



